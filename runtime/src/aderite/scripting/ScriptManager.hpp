#pragma once

#include <string>
#include <unordered_map>

#include <mono/jit/jit.h>

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
     * @brief Returns the current domain
     */
    MonoDomain* getDomain() const {
        return m_currentDomain;
    }

    /**
     * @brief Returns the image of the code assembly
     */
    MonoImage* getCodeImage() const {
        return m_codeImage;
    }

    /**
     * @brief Returns a system MonoClass instance
     * @param name Name of the system
     * @return System MonoClass instance or nullptr if a system with the specified name doesn't exist
     */
    MonoClass* getSystemClass(const std::string& name) const;

    /**
     * @brief Tries to resolve a class with the specified name
     * @param nSpace Namespace of the class
     * @param name Name of the class
     * @return MonoClass instance or nullptr if failed to resolve
     */
    MonoClass* resolveClass(const std::string& nSpace, const std::string& name);

    /**
     * @brief Instantiate the specified object (0 argument constructor)
     * @param klass Class to initialize
     * @return MonoObject instance
     */
    MonoObject* instantiate(MonoClass* klass);

    /**
     * @brief Returns public fields of the specified object
     * @param object Object instance
     * @return Vector of FieldWrapper objects
     */
    std::vector<FieldWrapper> getPublicFields(MonoObject* object);

    /**
     * @brief Tries to locate a method in the code assembly and returns it
     * @param klass Class to search in
     * @param signature Method signature
     * @param paramCount Number of arguments in the method
     * @return MonoMethod instance or nullptr
     */
    MonoMethod* getMethod(MonoClass* klass, const std::string& name, size_t paramCount);

    /**
     * @brief Tries to locate a method in the code assembly and returns it
     * @param signature Method signature
     * @return MonoMethod instance or nullptr
     */
    MonoMethod* getMethod(const std::string& signature);

    /**
     * @brief Returns a list of known systems and their names
     */
    std::unordered_map<std::string, MonoClass*> getKnownSystems() const {
        return m_knownSystems;
    }

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
    std::unordered_map<std::string, MonoClass*> m_knownSystems;
};

} // namespace scripting
} // namespace aderite
