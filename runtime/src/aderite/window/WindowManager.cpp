#include "WindowManager.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/Config.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/utility/Log.hpp"

#if GLFW_BACKEND
#ifdef ADERITE_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#error "Unsupported platform for glfw backend"
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#endif

namespace aderite {
namespace window {

// ---------------------------------
// PIMPL specification
// ---------------------------------

#if GLFW_BACKEND == 1

/**
 * @brief GLFW implementation of PlatformImpl
 */
class WindowManager::PlatformImpl {
public:
#ifdef ADERITE_PLATFORM_WINDOWS
    HWND NativeWindow;
#endif

    GLFWwindow* ImplementationWindow;
};

#endif

// ---------------------------------
// START OF SHARED FUNCTIONALITY
// ---------------------------------

bool WindowManager::init() {
    LOG_TRACE("Initializing window manager");
    m_impl = new PlatformImpl();

    if (!backendInit()) {
        LOG_ERROR("Failed to initialize backend");
        return false;
    }

    if (!backendCreateWindow()) {
        LOG_ERROR("Failed to create window");
        return false;
    }

    return true;
}

void WindowManager::shutdown() {
    backendShutdown();
    delete m_impl;
    m_impl = nullptr;
}

void* WindowManager::getNativeHandle() {
    return static_cast<void*>(m_impl->NativeWindow);
}

void* WindowManager::getImplementationHandle() {
    return static_cast<void*>(m_impl->ImplementationWindow);
}

// ---------------------------------
// START OF PLATFORM SPECIFIC FUNCTIONALITY
// ---------------------------------

#if GLFW_BACKEND == 1

void WindowManager::setSize(unsigned int width, unsigned int height) {
    glfwSetWindowSize(m_impl->ImplementationWindow, width, height);
}

glm::i32vec2 WindowManager::getSize() {
    glm::i32vec2 size = {};
    glfwGetWindowSize(m_impl->ImplementationWindow, &size.x, &size.y);
    return size;
}

bool WindowManager::isClosed() {
    return glfwWindowShouldClose(m_impl->ImplementationWindow) == 1;
}

void WindowManager::setTitle(const std::string& title) {
    glfwSetWindowTitle(m_impl->ImplementationWindow, title.c_str());
}

bool WindowManager::backendInit() {
    LOG_DEBUG("GLFW backend");

    glfwSetErrorCallback([](int error, const char* description) {
        LOG_ERROR("GLFW error: {0}, {1}", error, description);
    });

    if (!glfwInit()) {
        return false;
    }

    // BGFX should take care of the context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    return true;
}

bool WindowManager::backendCreateWindow() {
    m_impl->ImplementationWindow = glfwCreateWindow(1280, 720, "Aderite", NULL, NULL);

    if (!m_impl->ImplementationWindow) {
        LOG_ERROR("Failed to create window");
        return false;
    }

#ifdef ADERITE_PLATFORM_WINDOWS
    m_impl->NativeWindow = glfwGetWin32Window(m_impl->ImplementationWindow);
#endif

    return true;
}

void WindowManager::backendShutdown() {
    glfwDestroyWindow(m_impl->ImplementationWindow);
    glfwTerminate();
}

#endif

} // namespace window
} // namespace aderite
