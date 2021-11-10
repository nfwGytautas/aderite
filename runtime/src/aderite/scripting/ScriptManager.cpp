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
#include "aderite/scripting/InternalCalls.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
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

    for (scripting::ScriptSystem* system : currentScene->getScriptSystems()) {
        system->update(delta);
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
    this->resolveSystemNames();
}

MonoClass* ScriptManager::getSystemClass(const std::string& name) const {
    auto it = m_knownSystems.find(name);
    if (it == m_knownSystems.end()) {
        return nullptr;
    }
    return it->second;
}

MonoClass* ScriptManager::resolveClass(const std::string& nSpace, const std::string& name) {
    MonoClass* klass = mono_class_from_name(m_codeImage, nSpace.c_str(), name.c_str());
    if (klass == nullptr) {
        LOG_ERROR("Failed to find class {0} in namespace {1}", name, nSpace);
        return nullptr;
    }
    return klass;
}

MonoObject* ScriptManager::instantiate(MonoClass* klass) {
    // Create object
    MonoObject* object = mono_object_new(m_currentDomain, klass);

    // Invoke constructor
    mono_runtime_object_init(object);

    // Return instance
    return object;
}

std::vector<FieldWrapper> ScriptManager::getPublicFields(MonoObject* object) {
    std::vector<FieldWrapper> result;
    void* iter = NULL;
    MonoClassField* field;
    while (field = mono_class_get_fields(mono_object_get_class(object), &iter)) {
        if (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC) {
            result.push_back(FieldWrapper(field, object));
        }
    }
    return result;
}

MonoMethod* ScriptManager::getMethod(MonoClass* klass, const std::string& name, size_t paramCount) {
    MonoMethod* method = mono_class_get_method_from_name(klass, name.c_str(), paramCount);
    if (method == nullptr) {
        LOG_ERROR("Failed to find {0} method in {1}", name, mono_class_get_name(klass));
        return nullptr;
    }
    return method;
}

MonoMethod* ScriptManager::getMethod(const std::string& signature) {
    ADERITE_UNIMPLEMENTED;
    return nullptr;
}

FieldType ScriptManager::getType(MonoType* type) const {
    // Check for value types
    switch (mono_type_get_type(type)) {
    case MONO_TYPE_R4: {
        return FieldType::Float;
    }
    case MONO_TYPE_BOOLEAN: {
        return FieldType::Boolean;
    }
    default: {
        return m_locator.getType(type);
    }
    }
}

LibClassLocator& ScriptManager::getLocator() {
    return m_locator;
}

void ScriptManager::resolveSystemNames() {
    LOG_TRACE("Resolving systems");
    m_knownSystems.clear();

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

            if (m_locator.isSystem(monoClass)) {
                LOG_TRACE("Found system. Namespace: {1}, name: {0}", name, nSpace);
                m_knownSystems[nSpace + "." + name] = monoClass;
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
    if (!m_locator.locate(m_scriptlibImage)) {
        LOG_ERROR("Failed to locate engine classes");
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

void ScriptManager::clean() {
    // TODO: Invoke GC

    // TODO: Unload domain
}

} // namespace scripting
} // namespace aderite
