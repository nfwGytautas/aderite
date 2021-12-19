#pragma once

#include <string>
#include <unordered_map>

#include <mono/jit/jit.h>

#include "aderite/io/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/LibClassLocator.hpp"

namespace aderite {
class Engine;

namespace scripting {

/**
 * @brief The class responsible for handling all scripting related functionality
 */
class ScriptManager final {
public:
    /**
     * @brief Initializes the serializer
     * @return True if initialized without error, false otherwise
     */
    bool init();

    /**
     * @brief Shutdown serializer
     */
    void shutdown();

    /**
     * @brief Updates all scripts
     * @param delta Delta of the frame
     */
    void update(float delta);

    /**
     * @brief Loads the game code assembly
     */
    void loadAssemblies();

    /**
     * @brief Returns the list of known scripts
     */
    const std::vector<ScriptClass*>& getScripts() const;

    /**
     * @brief Returns script class with specified name or nullptr if doesn't exist
     * @param name Name of the script class
     */
    ScriptClass* getScript(const std::string& name) const;

    /**
     * @brief Get script event with the specified name, nullptr if doesn't exist
     * @param name Name of the script event
     * @return ScriptEvent instance
     */
    ScriptEvent* getEventFromName(const std::string& name) const;

    /**
     * @brief Invoked when a scene is changed
     * @param scene New scene
     */
    void onSceneChanged(scene::Scene* scene) const;

    /**
     * @brief Returns the current domain
     */
    MonoDomain* getDomain() const;

    /**
     * @brief Returns the image of the code assembly
     */
    MonoImage* getCodeImage() const;

    /**
     * @brief Create C# instance of a serializable, multiple calls with the same serializable will return the same object instance this way
     * saving on memory and time
     * @param serializable Serializable to create for
     * @return MonoObject instance
     */
    MonoObject* createInstance(io::SerializableObject* serializable);

    /**
     * @brief Tries to resolve a class with the specified name
     * @param nSpace Namespace of the class
     * @param name Name of the class
     * @return MonoClass instance or nullptr if failed to resolve
     */
    MonoClass* resolveClass(const std::string& nSpace, const std::string& name) const;

    /**
     * @brief Instantiate the specified object (0 argument constructor)
     * @param klass Class to initialize
     * @return MonoObject instance
     */
    MonoObject* instantiate(MonoClass* klass) const;

    /**
     * @brief Tries to locate a method in the code assembly and returns it
     * @param klass Class to search in
     * @param signature Method signature
     * @param paramCount Number of arguments in the method
     * @return MonoMethod instance or nullptr
     */
    MonoMethod* getMethod(MonoClass* klass, const std::string& name, size_t paramCount) const;

    /**
     * @brief Tries to locate a method in the code assembly and returns it
     * @param signature Method signature
     * @return MonoMethod instance or nullptr
     */
    MonoMethod* getMethod(const std::string& signature) const;

    /**
     * @brief Returns FieldType from the MonoType
     * @param type MonoType instance
     * @return Corresponding FieldType enum value
     */
    FieldType getType(MonoType* type) const;

    /**
     * @brief Returns the script lib locator
     * @return Locator reference
     */
    LibClassLocator& getLocator();

    /**
     * @brief Creates a MonoString from the specified value
     * @return MonoString object
     */
    MonoString* string(const char* value) const;

private:
    /**
     * @brief Resolves all system classes in the loaded assembly
     */
    void resolveSystemNames();

    /**
     * @brief Sets up all engine runtime related assemblies and information
     */
    bool setupEngineAssemblies();

    /**
     * @brief Sets up all code related assemblies and information
     */
    bool setupCodeAssemblies();

    /**
     * @brief Cleans up all data
     */
    void clean();

private:
    ScriptManager() {}
    friend Engine;

private:
    LibClassLocator m_locator;

    MonoDomain* m_jitDomain = nullptr;

    MonoDomain* m_currentDomain = nullptr;

    // Engine assemblies
    MonoAssembly* m_scriptlibAssembly = nullptr;
    MonoImage* m_scriptlibImage = nullptr;

    // Code assemblies
    MonoAssembly* m_codeAssembly = nullptr;
    MonoImage* m_codeImage = nullptr;

    // Vector containing the names of systems that exist in the image
    std::vector<ScriptClass*> m_scripts;
    std::unordered_map<io::SerializableObject*, MonoObject*> m_objectCache;
};

} // namespace scripting
} // namespace aderite
