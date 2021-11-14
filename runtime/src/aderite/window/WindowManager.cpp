#include "WindowManager.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/Config.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

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
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Window] Initializing window manager");
    m_impl = new PlatformImpl();

    if (!this->backendInit()) {
        LOG_ERROR("[Window] Failed to initialize backend");
        return false;
    }

    if (!this->backendCreateWindow()) {
        LOG_ERROR("[Window] Failed to create window");
        return false;
    }

    LOG_INFO("[Window] Window manager initialized");

    return true;
}

void WindowManager::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Window] Shutting down window manager");
    this->backendShutdown();
    delete m_impl;
    m_impl = nullptr;
    LOG_INFO("[Window] Window manager shutdown");
}

void* WindowManager::getNativeHandle() const {
    return static_cast<void*>(m_impl->NativeWindow);
}

void* WindowManager::getImplementationHandle() const {
    return static_cast<void*>(m_impl->ImplementationWindow);
}

// ---------------------------------
// START OF PLATFORM SPECIFIC FUNCTIONALITY
// ---------------------------------

#if GLFW_BACKEND == 1

void WindowManager::setSize(unsigned int width, unsigned int height) const {
    glfwSetWindowSize(m_impl->ImplementationWindow, width, height);
}

glm::i32vec2 WindowManager::getSize() const {
    glm::i32vec2 size = {};
    glfwGetWindowSize(m_impl->ImplementationWindow, &size.x, &size.y);
    return size;
}

bool WindowManager::isClosed() const {
    return glfwWindowShouldClose(m_impl->ImplementationWindow) == 1;
}

void WindowManager::setTitle(const std::string& title) const {
    glfwSetWindowTitle(m_impl->ImplementationWindow, title.c_str());
}

bool WindowManager::backendInit() const {
    LOG_DEBUG("[Window] GLFW backend");

    glfwSetErrorCallback([](int error, const char* description) {
        LOG_ERROR("[Window] GLFW error: {0}, {1}", error, description);
    });

    LOG_TRACE("[Window] Initializing GLFW");
    if (!glfwInit()) {
        LOG_ERROR("[Window] Failed to initialize GLFW");
        return false;
    }
    LOG_INFO("[Window] GLFW initialized");

    // BGFX should take care of the context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    LOG_INFO("[Window] Backend initialized");

    return true;
}

bool WindowManager::backendCreateWindow() const {
    LOG_TRACE("[Window] Creating window");
    m_impl->ImplementationWindow = glfwCreateWindow(1280, 720, "Aderite", NULL, NULL);

    if (!m_impl->ImplementationWindow) {
        LOG_ERROR("[Window] Failed to create window");
        return false;
    }

#ifdef ADERITE_PLATFORM_WINDOWS
    m_impl->NativeWindow = glfwGetWin32Window(m_impl->ImplementationWindow);
#endif

    LOG_INFO("[Window] Window created");

    return true;
}

void WindowManager::backendShutdown() const {
    LOG_TRACE("[Window] Shutting down backend");
    glfwDestroyWindow(m_impl->ImplementationWindow);
    glfwTerminate();
    LOG_INFO("[Window] Backend shutdown");
}

#endif

} // namespace window
} // namespace aderite
