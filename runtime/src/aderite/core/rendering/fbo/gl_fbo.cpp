#include "gl_fbo.hpp"

#include <glad/glad.h>

#include "aderite/utility/macros.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			gl_fbo::gl_fbo(const fbo_create_args& args) {
				glGenFramebuffers(1, &m_fbo);
				this->bind();

				// Texture image attachment
				glGenTextures(1, &m_tcolor);
				glBindTexture(GL_TEXTURE_2D, m_tcolor);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, args.width, args.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, 0);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tcolor, 0);

				// Depth and stencil
				glGenRenderbuffers(1, &m_dsrbo);
				glBindRenderbuffer(GL_RENDERBUFFER, m_dsrbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, args.width, args.height);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_dsrbo);

				// Check state
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					LOG_ERROR("Framebuffer is not complete");
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				width = args.width;
				height = args.height;
			}

			gl_fbo::~gl_fbo() {
				ASSERT_RENDER_THREAD;

				// Delete resources
				glDeleteTextures(1, &m_tcolor);
				glDeleteRenderbuffers(1, &m_dsrbo);
				glDeleteFramebuffers(1, &m_fbo);
			}

			void gl_fbo::bind() {
				glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
				glViewport(0, 0, width, height);
			}

		}
	}
}
