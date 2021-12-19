#include "FieldWrapper.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

FieldWrapper::FieldWrapper(MonoClassField* field) : m_field(field) {
    m_name = mono_field_get_name(field);
    m_type = ::aderite::Engine::getScriptManager()->getType(mono_field_get_type(field));
}

std::string FieldWrapper::getName() const {
    return m_name;
}

FieldType FieldWrapper::getType() const {
    return m_type;
}

void FieldWrapper::getValue(MonoObject* instance, void* value) const {
    mono_field_get_value(instance, m_field, value);
}

MonoObject* FieldWrapper::getValueObject(MonoObject* instance) const {
    return mono_field_get_value_object(::aderite::Engine::getScriptManager()->getDomain(), m_field, instance);
}

void FieldWrapper::setValue(MonoObject* instance, void* value) const {
    mono_field_set_value(instance, m_field, value);
}

io::SerializableAsset* FieldWrapper::getSerializable(MonoObject* instance) const {
    io::SerializableAsset* result = nullptr;

    // Get the mono object of the field first
    MonoObject* fieldValue = this->getValueObject(instance);

    // Check if set
    if (fieldValue == nullptr) {
        return nullptr;
    }

    // Get the instance part of the object
    MonoClassField* intanceField = mono_class_get_field_from_name(mono_object_get_class(fieldValue), "Instance");

    if (intanceField == nullptr) {
        return nullptr;
    }

    mono_field_get_value(fieldValue, intanceField, &result);

    // Return the result
    return result;
}

void FieldWrapper::setSerializable(MonoObject* instance, io::SerializableObject* serializable) const {
    this->setValue(instance, ::aderite::Engine::getScriptManager()->createInstance(serializable));
}

} // namespace scripting
} // namespace aderite
