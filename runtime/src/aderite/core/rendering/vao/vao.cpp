#include "vao.hpp"

#include "aderite/config.hpp"
#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/threading/threader.hpp"

#ifdef GL_BACKEND
#include "aderite/core/rendering/vao/gl_vao.hpp"
#endif

namespace aderite {

	vao* vao::create() {
		ASSERT_RENDER_THREAD;

#if GL_BACKEND
		return new render_backend::opengl::gl_vao();
#endif

		LOG_ERROR("No rendering backend specified");
		return nullptr;
	}

}
