#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief A wrapper for script classes
 */
class ScriptClass final {
public:
    ~ScriptClass();

    /**
     * @brief Creates or deletes and creates a new instances of this script
     */
    void reinstantiate();

    /**
     * @brief Returns the name of the script
     */
    const char* getName() const;

    /**
     * @brief Returns the instance of this script
     */
    MonoObject* getInstance() const;

    /**
     * @brief Returns an event with the specified name or nullptr if it doesn't exist
     * @param eventName Event name
     */
    ScriptEvent* getEvent(const std::string& eventName) const;

    /**
     * @brief Returns a list of update events in this script
     */
    const std::vector<ScriptUpdateEvent*>& getUpdateEvents() const;

private:
    ScriptClass(MonoClass* klass);
    friend class ScriptManager;

    /**
     * @brief Identifies methods of this class that can be used for callbacks
     */
    void locateMethods();

private:
    MonoClass* m_klass = nullptr;
    MonoObject* m_instance = nullptr;

    // Possible mappings
    std::vector<ScriptUpdateEvent*> m_updateEvents;
};

} // namespace scripting
} // namespace aderite
