#include "MonoUtils.hpp"
#include <sstream>

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include "aderite/scripting/FieldWrapper.hpp"
#include "aderite/utility/Log.hpp"

namespace aderite {
namespace scripting {

const std::string MethodSignature::value() const {
    std::stringstream ss;

    // Namespace
    if (!Namespace.empty()) {
        ss << Namespace;

        if (Namespace[Namespace.length() - 1] != '.') {
            ss << ".";
        }
    }

    // Class
    ss << Class;

    // Static
    if (IsStatic) {
        ss << "::";
    } else {
        ss << ":";
    }

    // Name
    ss << Name;

    // Parameters
    ss << "(";
    if (Parameters.size() > 0) {
        for (size_t i = 0; i < Parameters.size(); i++) {
            if (i != 0) {
                ss << ",";
            }

            ss << Parameters[i].Type;
        }
    }
    ss << ")";

    return ss.str();
}

void printClassMethods(MonoClass* klass) {
    void* iter = NULL;
    MonoMethod* method;
    LOG_TRACE("Methods for {0}:", mono_class_get_name(klass));
    while (method = mono_class_get_methods(klass, &iter)) {
        LOG_TRACE("\tMethod: {0}", mono_method_full_name(method, 1));
    }
}

void* unbox(MonoObject* object) {
    return mono_object_unbox(object);
}

std::string toString(MonoObject* object) {
    // TODO: Handle exception
    MonoString* pMonoStr = mono_object_to_string(object, nullptr);
    char* pStr = mono_string_to_utf8(pMonoStr);
    std::string str = reinterpret_cast<std::string::value_type*>(pStr);
    mono_free(pStr);
    return str;
}

} // namespace scripting
} // namespace aderite
