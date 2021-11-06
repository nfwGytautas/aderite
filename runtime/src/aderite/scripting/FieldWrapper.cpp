#include "FieldWrapper.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Log.hpp"

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

void FieldWrapper::getValue(MonoObject* instance, void* value) {
    mono_field_get_value(instance, m_field, value);
}

MonoObject* FieldWrapper::getValueObject(MonoObject* instance) {
    return mono_field_get_value_object(::aderite::Engine::getScriptManager()->getDomain(), m_field, instance);
}

void FieldWrapper::setValue(MonoObject* instance, void* value) {
    mono_field_set_value(instance, m_field, value);
}

} // namespace scripting
} // namespace aderite
