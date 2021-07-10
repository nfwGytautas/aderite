#include "fbo.hpp"

#include "aderite/config.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"

#ifdef GL_BACKEND
#include "aderite/core/rendering/fbo/gl_fbo.hpp"
#endif

namespace aderite {

	ref<fbo> fbo::create(const fbo_create_args& args) {
		ASSERT_RENDER_THREAD;

#if GL_BACKEND
		return new render_backend::opengl::gl_fbo(args);
#endif

		LOG_ERROR("No rendering backend specified");
		return nullptr;
	}

}
