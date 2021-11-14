#pragma once

#include <mono/jit/jit.h>

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

    ThunkedMethod(MonoMethod* method, MonoObject* instance) : m_method(method), m_instance(instance) {
        if (m_method != nullptr) {
            m_thunk = reinterpret_cast<ThunkFn>(mono_method_get_unmanaged_thunk(m_method));
        }
    }

    /**
     * @brief Set the exception handle of the thunked method
     */
    void setExceptionHandler(ExceptionHandler* handler) {
        m_handler = handler;
    }

    /**
     * @brief Invoke the thunked method with the given parameters
     */
    Ret invoke(Args... args) const {
        ADERITE_DYNAMIC_ASSERT(this->valid(), "Invalid thunked method invoked");
        MonoException* exception = nullptr;
        Ret result = m_thunk(m_instance, args..., &exception);
        if (exception != nullptr && m_handler != nullptr) {
            this->m_handler(exception);
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

    Ret operator()(Args... args) const {
        return this->invoke(args...);
    }

private:
    MonoMethod* m_method = nullptr;
    MonoObject* m_instance = nullptr;
    ThunkFn m_thunk = nullptr;
    ExceptionHandler* m_handler = nullptr;
};

/**
 * @brief void return specialization of ThunkedMethod
 * @tparam ...Args Arguments of the method
 */
template<typename... Args>
class ThunkedMethod<void, Args...> {
public:
    typedef void (*ThunkFn)(MonoObject*,Args...,MonoException**);
    using ExceptionHandler = void(MonoException* exception);

    ThunkedMethod() {}

    ThunkedMethod(MonoMethod* method, MonoObject* instance) : m_method(method), m_instance(instance) {
        if (m_method != nullptr) {
            m_thunk = reinterpret_cast<ThunkFn>(mono_method_get_unmanaged_thunk(m_method));
        }
    }

    /**
     * @brief Set the exception handle of the thunked method
     */
    void setExceptionHandler(ExceptionHandler* handler) {
        m_handler = handler;
    }

    /**
     * @brief Invoke the thunked method with the given parameters
     */
    void invoke(Args... args) const {
        ADERITE_DYNAMIC_ASSERT(this->valid(), "Invalid thunked method invoked");
        MonoException* exception = nullptr;
        m_thunk(m_instance, args..., &exception);
        if (exception != nullptr && m_handler != nullptr) {
            this->m_handler(exception);
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

    void operator()(Args... args) const {
        this->invoke(args...);
    }

private:
    MonoMethod* m_method = nullptr;
    MonoObject* m_instance = nullptr;
    ThunkFn m_thunk = nullptr;
    ExceptionHandler* m_handler = nullptr;
};

} // namespace scripting
} // namespace aderite
