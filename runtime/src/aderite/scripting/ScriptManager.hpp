#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

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
    MonoDomain* getDomain() const;

    /**
     * @brief Returns all behaviors that the script manager knows off
     */
    std::vector<BehaviorWrapper*> getAllBehaviors() const;

    /**
     * @brief Returns the behavior instance with the specified name
     * @param name Name of the behavior
     * @return BehaviorWrapper instance or nullptr if doesn't exist
     */
    BehaviorWrapper* getBehavior(const std::string& name);

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
     * @brief Resolves all Behavior classes in the loaded assembly
     */
    void resolveBehaviors();

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

    std::vector<BehaviorWrapper*> m_behaviors;
};

} // namespace scripting
} // namespace aderite
