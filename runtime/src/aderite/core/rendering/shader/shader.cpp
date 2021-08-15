#include "shader.hpp"

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/threading/threader.hpp"

#ifdef GL_BACKEND
#include "aderite/core/rendering/shader/gl_shader.hpp"
#endif

namespace aderite {

	shader* shader::create(const shader_create_args& args) {
		ASSERT_RENDER_THREAD;

#if GL_BACKEND
		return new render_backend::opengl::gl_shader(args);
#endif

		LOG_ERROR("No rendering backend specified");
		return nullptr;
	}

}
