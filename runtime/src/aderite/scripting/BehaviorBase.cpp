#include "BehaviorBase.hpp"

#include <mono/metadata/attrdefs.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/SerializableAsset.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/ScriptedBehavior.hpp"

namespace aderite {
namespace scripting {

BehaviorBase::BehaviorBase(MonoClass* klass) : m_klass(klass) {
    ScriptManager* sm = ::aderite::Engine::getScriptManager();

    // Resolve standard methods
    m_init = ThunkedMethod<void>(sm->getMethod(m_klass, "Initialize", 0));
    m_shutdown = ThunkedMethod<void>(sm->getMethod(m_klass, "Shutdown", 0));
    m_update = ThunkedMethod<void, float>(sm->getMethod(m_klass, "Update", 1));

    // Resolve physics methods
    m_triggerEnter = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerEnter", 1));
    m_triggerLeave = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerLeave", 1));
    m_triggerWasEntered = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerWasEntered", 1));
    m_triggerWasLeft = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnTriggerWasLeft", 1));
    m_collisionStart = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnCollisionStart", 1));
    m_collisionEnd = ThunkedMethod<void, MonoObject*>(sm->getMethod(m_klass, "OnCollisionEnd", 1));

    // Resolve public fields
    void* iter = NULL;
    MonoClassField* field;
    while (field = mono_class_get_fields(m_klass, &iter)) {
        if (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC) {
            m_fields.push_back(FieldWrapper(field));
        }
    }

    // Resolve the GameObject field
    m_instanceField = FieldWrapper(mono_class_get_field_from_name(m_klass, "Instance"));
}

const std::string BehaviorBase::getName() const {
    std::string result = mono_class_get_namespace(m_klass);
    if (!result.empty()) {
        result += ".";
    }
    result += mono_class_get_name(m_klass);
    return result;
}

const std::vector<FieldWrapper>& BehaviorBase::getFields() const {
    return m_fields;
}

bool BehaviorBase::hasField(const std::string& name) const {
    return std::find_if(m_fields.begin(), m_fields.end(), [name](const FieldWrapper& wrapper) {
               return wrapper.getName() == name;
           }) != m_fields.end();
}

const FieldWrapper& BehaviorBase::getField(const std::string& name) const {
    return *std::find_if(m_fields.begin(), m_fields.end(), [name](const FieldWrapper& wrapper) {
        return wrapper.getName() == name;
    });
}

MonoClass* BehaviorBase::getClass() const {
    return m_klass;
}

void BehaviorBase::copyOver(ScriptedBehavior* source, ScriptedBehavior* dst) {
    if (source->getBase() != dst->getBase() || source->getBase() != this) {
        ADERITE_ABORT("[Scripting] BehaviorBase mismatch");
    }

    // Copy field information
    for (FieldWrapper& fw : m_fields) {
        switch (fw.getType()) {
        case FieldType::Integer: {
            int val = 0;
            fw.getValue(source->getInstance(), &val);
            fw.setValue(dst->getInstance(), &val);
            break;
        }
        case FieldType::Float: {
            int val = 0;
            fw.getValue(source->getInstance(), &val);
            fw.setValue(dst->getInstance(), &val);
            break;
        }
        case FieldType::Boolean: {
            bool val = false;
            fw.getValue(source->getInstance(), &val);
            fw.setValue(dst->getInstance(), &val);
            break;
        }
        case FieldType::Material:
        case FieldType::Prefab:
        case FieldType::Audio:
        case FieldType::Mesh: {
            io::SerializableAsset* asset = fw.getSerializable(source->getInstance());
            fw.setSerializable(dst->getInstance(), asset);
            break;
        }
        }
    }
}

} // namespace scripting
} // namespace aderite
