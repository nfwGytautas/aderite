#include "FieldWrapper.hpp"

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

FieldWrapper::FieldWrapper(MonoClassField* field, MonoObject* instance) : m_field(field), m_instance(instance) {
    m_name = mono_field_get_name(field);
    m_type = ::aderite::Engine::getScriptManager()->getType(mono_field_get_type(field));
}

std::string FieldWrapper::getName() const {
    return m_name;
}

FieldType FieldWrapper::getType() const {
    return m_type;
}

void FieldWrapper::getValue(void* value) const {
    mono_field_get_value(m_instance, m_field, value);
}

MonoObject* FieldWrapper::getValueObject() const {
    return mono_field_get_value_object(::aderite::Engine::getScriptManager()->getDomain(), m_field, m_instance);
}

void FieldWrapper::setValue(void* value) const {
    mono_field_set_value(m_instance, m_field, value);
}

io::ISerializable* FieldWrapper::getSerializable() const {
    io::ISerializable* result = nullptr;

    // Get the mono object of the field first
    MonoObject* fieldValue = this->getValueObject();

    // Check if set
    if (fieldValue == nullptr) {
        return nullptr;
    }

    // Get the instance part of the object
    MonoClassField* intanceField = mono_class_get_field_from_name(mono_object_get_class(fieldValue), "Instance");
    mono_field_get_value(m_instance, m_field, &result);

    // Return the result
    return result;
}

void FieldWrapper::setSerializable(io::ISerializable* serializable) const {
    this->setValue(::aderite::Engine::getScriptManager()->createInstance(serializable));
}

} // namespace scripting
} // namespace aderite
