#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/scripting/FieldWrapper.hpp"
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
     * @brief Create a new instance of this script
     */
    MonoObject* createInstance() const;

    /**
     * @brief Load the data of this class (data ownership is not taken, but the relative data is set)
     * @param data ScriptData instance
     */
    void loadData(ScriptData* data);

    /**
     * @brief Returns the name of the script
     */
    const char* getName() const;

    /**
     * @brief Returns the current instance of this script
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

    /**
     * @brief Returns a list of field in this script
     */
    const std::vector<FieldWrapper>& getFields() const;

private:
    ScriptClass(MonoClass* klass);
    friend class ScriptManager;

    /**
     * @brief Identifies methods of this class that can be used for callbacks
     */
    void locateMethods();

    /**
     * @brief Identifies fields of this class that can be set
     */
    void locateFields();

private:
    MonoClass* m_klass = nullptr;
    MonoObject* m_currentInstance = nullptr;

    // Fields
    std::vector<FieldWrapper> m_fields;

    // Possible mappings
    std::vector<ScriptUpdateEvent*> m_updateEvents;
};

} // namespace scripting
} // namespace aderite
