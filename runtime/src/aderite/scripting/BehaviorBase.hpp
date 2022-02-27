#pragma once

#include <vector>

#include <mono/jit/jit.h>

#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/scripting/ThunkedMethod.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Class used to wrap around a scripted behavior, this acts as a base changeable for ScriptedBehavior class using composition
 */
class BehaviorBase final {
public:
    /**
     * @brief Create BehaviorBase class from MonoClass object
     * @param klass MonoClass object
     */
    BehaviorBase(MonoClass* klass);

    /**
     * @brief Returns the full name of this behavior
     */
    const std::string getName() const;

    /**
     * @brief Returns the public fields of this system
     */
    const std::vector<FieldWrapper>& getFields() const;

    /**
     * @brief Returns true if the system has a field with the specified name
     * @param name Name of the field
     */
    bool hasField(const std::string& name) const;

    /**
     * @brief Returns the field wrapper with the specified name
     * @param name Name of the field
     */
    const FieldWrapper& getField(const std::string& name) const;

    /**
     * @brief Returns the MonoClass of this behavior
     */
    MonoClass* getClass() const;

    /**
     * @brief Copies field information from the specified source behavior to the specified destination behavior
     * @param source Source behavior to copy information from
     * @param dst Destination behavior to copy information to
     */
    void copyOver(ScriptedBehavior* source, ScriptedBehavior* dst);

private:
    // The C# class representation
    MonoClass* m_klass = nullptr;

    // Instance field
    FieldWrapper m_instanceField;

    // Fields of this behavior
    std::vector<FieldWrapper> m_fields;

    // Standard script methods
    ThunkedMethod<void> m_init;
    ThunkedMethod<void, float> m_update;
    ThunkedMethod<void> m_shutdown;

    // Physics methods
    ThunkedMethod<void, MonoObject*> m_triggerEnter;
    ThunkedMethod<void, MonoObject*> m_triggerLeave;
    ThunkedMethod<void, MonoObject*> m_triggerWasEntered;
    ThunkedMethod<void, MonoObject*> m_triggerWasLeft;
    ThunkedMethod<void, MonoObject*> m_collisionStart;
    ThunkedMethod<void, MonoObject*> m_collisionEnd;

    friend class ScriptedBehavior;
};

} // namespace scripting
} // namespace aderite
