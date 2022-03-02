#pragma once

#include <mono/jit/jit.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/ScriptManager.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Helper class for thunked mono methods
 * @tparam Ret Return type
 * @tparam ...Args Arguments
 */
template<typename Ret, typename... Args>
class ThunkedMethod {
public:
    typedef void (*ThunkFn)(MonoObject*, Args..., MonoException**);
    using ExceptionHandler = void(MonoException* exception);

    ThunkedMethod() {}

    ThunkedMethod(MonoMethod* method) : m_method(method) {
        if (m_method != nullptr) {
            m_thunk = reinterpret_cast<ThunkFn>(mono_method_get_unmanaged_thunk(m_method));
        }
    }

    /**
     * @brief Invoke the thunked method with the given parameters
     */
    Ret invoke(MonoObject* instance, Args... args) const {
        ADERITE_DYNAMIC_ASSERT(this->valid(), "Invalid thunked method invoked");
        MonoException* exception = nullptr;
        Ret result = m_thunk(instance, args..., &exception);
        if (exception != nullptr) {
            ::aderite::Engine::getScriptManager()->onScriptException(exception);
        }
        return result;
    }

    /**
     * @brief Returns true if the thunked method is valid, false otherwise
     */
    bool valid() const {
        return m_method != nullptr && m_thunk != nullptr;
    }

    operator bool() const {
        return this->valid();
    }

    Ret operator()(MonoObject* instance, Args... args) const {
        return this->invoke(instance, args...);
    }

private:
    MonoMethod* m_method = nullptr;
    ThunkFn m_thunk = nullptr;
};

/**
 * @brief void return specialization of ThunkedMethod
 * @tparam ...Args Arguments of the method
 */
template<typename... Args>
class ThunkedMethod<void, Args...> {
public:
    typedef void (*ThunkFn)(MonoObject*, Args..., MonoException**);

    ThunkedMethod() {}

    ThunkedMethod(MonoMethod* method) : m_method(method) {
        if (m_method != nullptr) {
            m_thunk = reinterpret_cast<ThunkFn>(mono_method_get_unmanaged_thunk(m_method));
        }
    }

    /**
     * @brief Invoke the thunked method with the given parameters
     */
    void invoke(MonoObject* instance, Args... args) const {
        ADERITE_DYNAMIC_ASSERT(this->valid(), "Invalid thunked method invoked");
        MonoException* exception = nullptr;
        m_thunk(instance, args..., &exception);
        if (exception != nullptr) {
            ::aderite::Engine::getScriptManager()->onScriptException(exception);
        }
    }

    /**
     * @brief Returns true if the thunked method is valid, false otherwise
     */
    bool valid() const {
        return m_method != nullptr && m_thunk != nullptr;
    }

    operator bool() const {
        return this->valid();
    }

    void operator()(MonoObject* instance, Args... args) const {
        this->invoke(instance, args...);
    }

private:
    MonoMethod* m_method = nullptr;
    ThunkFn m_thunk = nullptr;
};

} // namespace scripting
} // namespace aderite
