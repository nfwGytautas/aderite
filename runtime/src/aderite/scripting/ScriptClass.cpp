#include "ScriptClass.hpp"

#include <mono/metadata/attrdefs.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptData.hpp"
#include "aderite/scripting/ScriptEvent.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/scripting/events/ScriptUpdateEvent.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scripting {

inline bool isUpdateSignature(MonoMethodSignature* signature) {
    // Update method:
    //  return void
    //  1 parameter (float)

    const uint32_t paramCount = mono_signature_get_param_count(signature);
    MonoType* retType = mono_signature_get_return_type(signature);

    std::vector<MonoType*> params;

    void* iter = NULL;
    MonoType* param;
    while (param = mono_signature_get_params(signature, &iter)) {
        params.push_back(param);
    }

    if (mono_type_get_class(retType) != nullptr) {
        // Non void return
        return false;
    }

    if (params.size() != 1) {
        // More than 1 parameter
        return false;
    }

    if (mono_type_get_type(params[0]) != MONO_TYPE_R4) {
        // 1 parameter not float
        return false;
    }

    return true;
}

ScriptClass::~ScriptClass() {
    for (ScriptUpdateEvent* sue : m_updateEvents) {
        delete sue;
    }
}

MonoObject* ScriptClass::createInstance() const {
    return ::aderite::Engine::getScriptManager()->instantiate(m_klass);
}

void ScriptClass::loadData(ScriptData* data) {
    ADERITE_DYNAMIC_ASSERT(std::string(this->getName()) == data->getScriptName(), "Incompatible script and data instance");

    m_currentInstance = data->getInstance();
}

const char* ScriptClass::getName() const {
    return mono_class_get_name(m_klass);
}

MonoObject* ScriptClass::getInstance() const {
    return m_currentInstance;
}

ScriptEvent* ScriptClass::getEvent(const std::string& eventName) const {
    for (ScriptUpdateEvent* sue : m_updateEvents) {
        if (sue->getName() == eventName) {
            return sue;
        }
    }

    return nullptr;
}

const std::vector<ScriptUpdateEvent*>& ScriptClass::getUpdateEvents() const {
    return m_updateEvents;
}

const std::vector<FieldWrapper>& ScriptClass::getFields() const {
    return m_fields;
}

ScriptClass::ScriptClass(MonoClass* klass) : m_klass(klass) {
    ADERITE_DYNAMIC_ASSERT(klass != nullptr, "Null class passed to ScriptClass constructor");
    this->locateMethods();
    this->locateFields();
}

void ScriptClass::locateMethods() {
    void* iter = NULL;
    MonoMethod* method;
    while (method = mono_class_get_methods(m_klass, &iter)) {
        MonoMethodSignature* signature = mono_method_signature(method);

        // Analyze signature
        if (isUpdateSignature(signature)) {
            m_updateEvents.push_back(new ScriptUpdateEvent(this, method));
        }
    }
}

void ScriptClass::locateFields() {
    // Iterate over public fields and set them, those that don't exist do nothing
    void* iter = NULL;
    MonoClassField* field;
    while (field = mono_class_get_fields(m_klass, &iter)) {
        if (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC) {
            m_fields.push_back(FieldWrapper(field));
        }
    }
}

} // namespace scripting
} // namespace aderite
