#include "ScriptManager.hpp"

#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scripting/InternalCalls.hpp"
#include "aderite/scripting/ScriptSystem.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

namespace aderite {
namespace scripting {

bool ScriptManager::init() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Scripting] Initializing script manager");

    // Set directories
    // TODO: Change this to dependencies
    const char* libDir = "C:/Program Files/Mono/lib/";
    const char* etcDir = "C:/Program Files/Mono/etc/";
    LOG_DEBUG("[Scripting] Setting directories to {0} {1}", libDir, etcDir);
    mono_set_dirs(libDir, etcDir);

    // Create domain
    m_jitDomain = mono_jit_init("AderiteJitDomain");
    if (!m_jitDomain) {
        LOG_ERROR("[Scripting] Failed to initialize mono jit");
        return false;
    }

    // Link functions
    linkInternals();

    LOG_INFO("[Scripting] Script manager initialized");

    return true;
}

void ScriptManager::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Scripting] Shutting down script manager");
    this->clean();

    // Clean all objects
    mono_gc_collect(mono_gc_max_generation());

    mono_jit_cleanup(m_jitDomain);
    // mono_domain_unload(m_jitDomain);

    LOG_INFO("[Scripting] Script manager shutdown");
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
    LOG_TRACE("[Scripting] Loading assemblies");
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
        LOG_ERROR("[Scripting] Couldn't setup engine assemblies");
        return;
    }

    if (!this->setupCodeAssemblies()) {
        LOG_ERROR("[Scripting] Couldn't setup code assemblies");
        return;
    }

    // Get behaviors
    this->resolveSystemNames();

    LOG_INFO("[Scripting] Assemblies loaded");
}

MonoDomain* ScriptManager::getDomain() const {
    return m_currentDomain;
}

MonoImage* ScriptManager::getCodeImage() const {
    return m_codeImage;
}

MonoObject* ScriptManager::createInstance(io::ISerializable* serializable) {
    ADERITE_DYNAMIC_ASSERT(serializable != nullptr, "Nullptr serializable passed to createInstance");

    // Check if an object already exists
    auto it = m_objectCache.find(serializable);
    if (it != m_objectCache.end()) {
        // Already have instance
        return it->second;
    }

    // Create instance
    MonoObject* instance = m_locator.create(serializable);
    m_objectCache[serializable] = instance;
    return instance;
}

MonoClass* ScriptManager::getSystemClass(const std::string& name) const {
    auto it = m_knownSystems.find(name);
    if (it == m_knownSystems.end()) {
        return nullptr;
    }
    return it->second;
}

MonoClass* ScriptManager::resolveClass(const std::string& nSpace, const std::string& name) const {
    MonoClass* klass = mono_class_from_name(m_codeImage, nSpace.c_str(), name.c_str());
    if (klass == nullptr) {
        LOG_ERROR("[Scripting] Failed to find class {0} in namespace {1}", name, nSpace);
        return nullptr;
    }
    return klass;
}

MonoObject* ScriptManager::instantiate(MonoClass* klass) const {
    // Create object
    MonoObject* object = mono_object_new(m_currentDomain, klass);

    // Invoke constructor
    mono_runtime_object_init(object);

    // Return instance
    return object;
}

std::vector<FieldWrapper> ScriptManager::getPublicFields(MonoObject* object) const {
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

MonoMethod* ScriptManager::getMethod(MonoClass* klass, const std::string& name, size_t paramCount) const {
    MonoMethod* method = mono_class_get_method_from_name(klass, name.c_str(), paramCount);
    if (method == nullptr) {
        LOG_ERROR("[Scripting] Failed to find {0} method in {1}", name, mono_class_get_name(klass));
        return nullptr;
    }
    return method;
}

MonoMethod* ScriptManager::getMethod(const std::string& signature) const {
    ADERITE_UNIMPLEMENTED;
    return nullptr;
}

std::unordered_map<std::string, MonoClass*> ScriptManager::getKnownSystems() const {
    return m_knownSystems;
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

MonoString* ScriptManager::string(const char* value) const {
    return mono_string_new(m_currentDomain, value);
}

void ScriptManager::resolveSystemNames() {
    LOG_TRACE("[Scripting] Resolving systems");
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
                LOG_TRACE("[Scripting] Found system. Namespace: {1}, name: {0}", name, nSpace);
                m_knownSystems[nSpace + "." + name] = monoClass;
            }
        }
    }
}

bool ScriptManager::setupEngineAssemblies() {
    LOG_TRACE("[Scripting] Setting up engine scriptlib assemblies");

    io::DataChunk assemblyChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::ScriptLibCode);

    // Create image
    LOG_TRACE("[Scripting] Loading engine image");
    MonoImageOpenStatus status;
    m_scriptlibImage = mono_image_open_from_data_with_name(reinterpret_cast<char*>(assemblyChunk.Data.data()), assemblyChunk.Data.size(), 1,
                                                           &status, 0, "ScriptLibImage");

    if (status != MONO_IMAGE_OK || m_scriptlibImage == nullptr) {
        LOG_WARN("[Scripting] Image could not be created");
        return false;
    }
    LOG_INFO("[Scripting] Engine image loaded");

    // Load assembly
    LOG_TRACE("[Scripting] Loading engine assembly");
    m_scriptlibAssembly = mono_assembly_load_from_full(m_scriptlibImage, "ScriptLibAssembly", &status, false);
    if (status != MONO_IMAGE_OK || m_scriptlibAssembly == nullptr) {
        mono_image_close(m_scriptlibImage);
        LOG_WARN("[Scripting] Assembly could not be loaded");
        return false;
    }
    LOG_INFO("[Scripting] Engine assembly loaded");

    // Find meta classes
    if (!m_locator.locate(m_scriptlibImage)) {
        LOG_ERROR("[Scripting] Failed to locate engine classes");
        return false;
    }

    LOG_INFO("[Scripting] Engine assemblies setup");

    return true;
}

bool ScriptManager::setupCodeAssemblies() {
    LOG_TRACE("[Scripting] Setting up game code assemblies");

    io::DataChunk assemblyChunk = ::aderite::Engine::getFileHandler()->openReservedLoadable(io::FileHandler::Reserved::GameCode);

    // Create image
    LOG_TRACE("[Scripting] Loading code image");
    MonoImageOpenStatus status;
    m_codeImage = mono_image_open_from_data_with_name(reinterpret_cast<char*>(assemblyChunk.Data.data()), assemblyChunk.Data.size(), 1,
                                                      &status, 0, "CodeImage");

    if (status != MONO_IMAGE_OK || m_codeImage == nullptr) {
        LOG_WARN("[Scripting] Image could not be created");
        return false;
    }
    LOG_INFO("[Scripting] Code image loaded");

    // Load assembly
    LOG_TRACE("[Scripting] Loading code assembly");
    m_codeAssembly = mono_assembly_load_from_full(m_codeImage, "CodeAssembly", &status, false);
    if (status != MONO_IMAGE_OK || m_codeAssembly == nullptr) {
        mono_image_close(m_codeImage);
        LOG_WARN("[Scripting] Assembly could not be loaded");
        return false;
    }
    LOG_INFO("[Scripting] Code assembly loaded");

    LOG_INFO("[Scripting] Code assemblies setup");

    return true;
}

void ScriptManager::clean() {
    m_objectCache.clear();

    // TODO: Invoke GC

    // TODO: Unload domain
}

} // namespace scripting
} // namespace aderite
