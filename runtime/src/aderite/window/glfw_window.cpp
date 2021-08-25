#include "glfw_window.hpp"

// TODO: Config
#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "aderite/utility/Log.hpp"

ADERITE_WINDOW_NAMESPACE_BEGIN

glfw_window::glfw_window(CreateOptions options)
	: m_window(nullptr)
{
	m_window = glfwCreateWindow(options.width, options.height, options.title, NULL, NULL);
}

glm::vec2 glfw_window::getSize() {
	int width, height = 0;
	glfwGetFramebufferSize(m_window, &width, &height);
	return { width, height };
}

void glfw_window::setTitle(const std::string& title) {
	glfwSetWindowTitle(m_window, title.c_str());
}

void* glfw_window::getNativeHandle() {
	// TODO: Detect platform
	return glfwGetWin32Window(m_window);
}

void glfw_window::destroy() {
	glfwDestroyWindow(m_window);
}

void glfw_window::makeActive() {
	//glfwMakeContextCurrent(m_window);
	Window::makeActive();
}

void glfw_window::beginFrame() {

}

void glfw_window::endFrame() {
	closed = glfwWindowShouldClose(m_window);

	// BGFX should take care of this
	
	// Swap buffer
	//glfwSwapBuffers(m_window);
}

bool init_backend(WindowManager* ws) {
	LOG_DEBUG("GLFW backend");

	glfwSetErrorCallback([](int error, const char* description) {
		LOG_ERROR("GLFW error: {0}, {1}", error, description);
	});

	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// BGFX should take care of the context
	// 
//#ifndef __DEBUG
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//
//	// Debug mode for OpenGL
//	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
//#else
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//#endif
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return true;
}

void shutdown_backend(WindowManager* ws) {
	glfwTerminate();
}

void poll_events(WindowManager* ws) {
	glfwPollEvents();
}

ADERITE_WINDOW_NAMESPACE_END