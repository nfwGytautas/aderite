#include "ScriptManager.hpp"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/scripting/BehaviorWrapper.hpp"
#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/InternalCalls.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

bool ScriptManager::init() {
    LOG_TRACE("Initializing script manager");

    // Set directories
    // TODO: Change this to dependencies
    const char* libDir = "C:/Program Files/Mono/lib/";
    const char* etcDir = "C:/Program Files/Mono/etc/";
    LOG_DEBUG("Setting directories to {0} {1}", libDir, etcDir);
    mono_set_dirs(libDir, etcDir);

    // Create domain
    m_jitDomain = mono_jit_init("AderiteJitDomain");
    if (!m_jitDomain) {
        LOG_ERROR("Failed to initialize mono jit");
        return false;
    }

    // Link functions
    linkInternals();

    return true;
}

void ScriptManager::shutdown() {
    this->clean();

    // Clean all objects
    mono_gc_collect(mono_gc_max_generation());

    mono_jit_cleanup(m_jitDomain);
    // mono_domain_unload(m_jitDomain);
}

void ScriptManager::update(float delta) {
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();

    if (currentScene == nullptr) {
        return;
    }

    auto scriptableView = currentScene->getEntityRegistry().view<scene::ScriptsComponent>();
    for (auto entity : scriptableView) {
        auto [script] = scriptableView.get(entity);
        script.Scripts->update(delta);
    }
}

void ScriptManager::loadAssemblies() {
    io::DataChunk assemblyChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::GameCode);

    if (assemblyChunk.Data.size() == 0) {
        // Empty nothing to load
        return;
    }

    if (m_currentDomain != nullptr) {
        this->clean();
    }

    // Create domain
    m_currentDomain = mono_domain_create_appdomain(const_cast<char*>("AderiteCodeDomain"), nullptr);

    // Set the domain
    if (mono_domain_set(m_currentDomain, 0)) {
        mono_thread_attach(m_currentDomain);
    }

    if (!this->setupEngineAssemblies()) {
        return;
    }

    if (!this->setupCodeAssemblies()) {
        return;
    }

    // Get behaviors
    this->resolveBehaviors();
}

BehaviorWrapper* ScriptManager::getBehavior(const std::string& name) {
    auto it = std::find_if(m_behaviors.begin(), m_behaviors.end(), [name](BehaviorWrapper* bw) {
        return bw->getName() == name;
    });

    if (it == m_behaviors.end()) {
        LOG_WARN("Failed to find behavior with name {0}", name);
        return nullptr;
    }

    return *it;
}

MonoObject* ScriptManager::createEntityObject(scene::Entity entity) {
    MonoObject* object = mono_object_new(m_currentDomain, m_entityClass);
    entt::entity e = entity.getHandle();
    scene::Scene* scene = entity.getScene();

    mono_runtime_object_init(object);

    mono_field_set_value(object, m_entitySceneField, &scene);
    mono_field_set_value(object, m_entityEntityField, &e);

    return object;
}

MonoObject* ScriptManager::createMeshObject(asset::MeshAsset* mesh) {
    // TODO: Handle exception
    void* args[1];
    args[0] = &mesh;
    MonoObject* object = mono_object_new(m_currentDomain, m_meshClass);
    MonoObject* ex = nullptr;
    mono_runtime_invoke(m_meshCtor, object, args, &ex);
    return object;
}

MonoObject* ScriptManager::createMaterialObject(asset::MaterialAsset* material) {
    // TODO: Handle exception
    void* args[1];
    args[0] = &material;
    MonoObject* object = mono_object_new(m_currentDomain, m_materialClass);
    MonoObject* ex = nullptr;
    mono_runtime_invoke(m_materialCtor, object, args, &ex);
    return object;
}

MonoObject* ScriptManager::createAudioObject(asset::AudioAsset* audio) {
    // TODO: Handle exception
    void* args[1];
    args[0] = &audio;
    MonoObject* object = mono_object_new(m_currentDomain, m_audioClass);
    MonoObject* ex = nullptr;
    mono_runtime_invoke(m_audioCtor, object, args, &ex);
    return object;
}

MonoClassField* ScriptManager::getBehaviorEntityField() const {
    return m_sbEntityField;
}

void ScriptManager::resolveBehaviors() {
    LOG_TRACE("Resolving behaviors");

    // Iterate over all classes and check if they have ScriptedBehavior attribute on them

    // Get the number of rows in the metadata table
    int numRows = mono_image_get_table_rows(m_codeImage, MONO_TABLE_TYPEDEF);

    for (int i = 0; i < numRows; i++) {
        // Get class
        MonoClass* monoClass = mono_class_get(m_codeImage, (i + 1) | MONO_TOKEN_TYPE_DEF);

        // Check if the class is found
        if (monoClass != nullptr) {
            std::string name = mono_class_get_name(monoClass);
            std::string nSpace = mono_class_get_namespace(monoClass);

            if (name == "<Module>" || name == "Internal") {
                // Ignore
                continue;
            }

            if (this->classMarkedAsBehavior(monoClass)) {
                LOG_TRACE("Found behavior. Namespace: {1}, name: {0}", name, nSpace);
                m_behaviors.push_back(new BehaviorWrapper(m_codeImage, monoClass));
            }
        }
    }
}

bool ScriptManager::setupEngineAssemblies() {
    LOG_TRACE("Setting up engine scriptlib assemblies");

    io::DataChunk assemblyChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::ScriptLibCode);

    // Create image
    MonoImageOpenStatus status;
    m_scriptlibImage = mono_image_open_from_data_with_name(reinterpret_cast<char*>(assemblyChunk.Data.data()), assemblyChunk.Data.size(), 1,
                                                           &status, 0, "ScriptLibImage");

    if (status != MONO_IMAGE_OK || m_scriptlibImage == nullptr) {
        LOG_WARN("Image could not be created");
        return false;
    }

    // Load assembly
    m_scriptlibAssembly = mono_assembly_load_from_full(m_scriptlibImage, "ScriptLibAssembly", &status, false);
    if (status != MONO_IMAGE_OK || m_scriptlibAssembly == nullptr) {
        mono_image_close(m_scriptlibImage);
        LOG_WARN("Assembly could not be loaded");
        return false;
    }

    // Find meta classes
    m_sbClass = mono_class_from_name(m_scriptlibImage, "Aderite", "ScriptedBehavior");
    if (m_sbClass == nullptr) {
        LOG_ERROR("Failed to find ScriptedBehavior class");
        return false;
    }

    m_sbEntityField = mono_class_get_field_from_name(m_sbClass, "Entity");
    if (m_sbEntityField == nullptr) {
        LOG_ERROR("Failed to find ScriptedBehavior.Entity field");
        return false;
    }

    m_entityClass = mono_class_from_name(m_scriptlibImage, "Aderite", "Entity");
    if (m_entityClass == nullptr) {
        LOG_ERROR("Failed to find Entity class");
        return false;
    }

    m_entitySceneField = mono_class_get_field_from_name(m_entityClass, "scene");
    if (m_entitySceneField == nullptr) {
        LOG_ERROR("Failed to find Entity.scene field");
        return false;
    }

    m_entityEntityField = mono_class_get_field_from_name(m_entityClass, "entity");
    if (m_entityEntityField == nullptr) {
        LOG_ERROR("Failed to find Entity.entity field");
        return false;
    }

    m_meshClass = mono_class_from_name(m_scriptlibImage, "Aderite", "Mesh");
    if (m_meshClass == nullptr) {
        LOG_ERROR("Failed to find Mesh class");
        return false;
    }

    m_meshCtor = mono_class_get_method_from_name(m_meshClass, ".ctor", 1);
    if (m_meshCtor == nullptr) {
        LOG_ERROR("Failed to find Mesh constructor method");
        return false;
    }

    m_materialClass = mono_class_from_name(m_scriptlibImage, "Aderite", "Material");
    if (m_materialClass == nullptr) {
        LOG_ERROR("Failed to find Material class");
        return false;
    }

    m_materialCtor = mono_class_get_method_from_name(m_materialClass, ".ctor", 1);
    if (m_materialCtor == nullptr) {
        LOG_ERROR("Failed to find Material constructor method");
        return false;
    }

    m_audioClass = mono_class_from_name(m_scriptlibImage, "Aderite", "Audio");
    if (m_audioClass == nullptr) {
        LOG_ERROR("Failed to find Audio class");
        return false;
    }

    m_audioCtor = mono_class_get_method_from_name(m_audioClass, ".ctor", 1);
    if (m_audioCtor == nullptr) {
        LOG_ERROR("Failed to find Audio constructor method");
        return false;
    }

    return true;
}

bool ScriptManager::setupCodeAssemblies() {
    LOG_TRACE("Setting up game code assemblies");

    io::DataChunk assemblyChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::GameCode);

    // Create image
    MonoImageOpenStatus status;
    m_codeImage = mono_image_open_from_data_with_name(reinterpret_cast<char*>(assemblyChunk.Data.data()), assemblyChunk.Data.size(), 1,
                                                      &status, 0, "CodeImage");

    if (status != MONO_IMAGE_OK || m_codeImage == nullptr) {
        LOG_WARN("Image could not be created");
        return false;
    }

    // Load assembly
    m_codeAssembly = mono_assembly_load_from_full(m_codeImage, "CodeAssembly", &status, false);
    if (status != MONO_IMAGE_OK || m_codeAssembly == nullptr) {
        mono_image_close(m_codeImage);
        LOG_WARN("Assembly could not be loaded");
        return false;
    }

    return true;
}

bool ScriptManager::classMarkedAsBehavior(MonoClass* klass) {
    //// Get attributes
    // MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_class(klass);

    //// Check if there are any attributes
    // if (attrInfo == nullptr) {
    //	return false;
    //}

    //// Iterate over attributes
    // for (int i = 0; i < attrInfo->num_attrs; i++) 	{
    //	// Get attribute class
    //	MonoClass* attrClass = mono_method_get_class(attrInfo->attrs[i].ctor);

    //	if (attrClass == m_sbAttributeClass) {
    //		mono_custom_attrs_free(attrInfo);
    //		return true;
    //	}
    //}

    //// Free memory
    // mono_custom_attrs_free(attrInfo);

    //// Return the attributes
    // return false;

    if (mono_class_get_parent(klass) == m_sbClass) {
        return true;
    }

    return false;
}

void ScriptManager::clean() {
    // TODO: Don't delete just reconfigure its meta data
    for (BehaviorWrapper* bw : m_behaviors) {
        delete bw;
        bw = nullptr;
    }

    m_behaviors.clear();

    // TODO: Invoke GC

    // TODO: Unload domain
}

MonoDomain* ScriptManager::getDomain() const {
    return m_currentDomain;
}

std::vector<BehaviorWrapper*> ScriptManager::getAllBehaviors() const {
    return m_behaviors;
}

} // namespace scripting
} // namespace aderite
