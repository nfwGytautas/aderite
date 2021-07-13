#include "gl_shader.hpp"

#include <glad/glad.h>

#include "aderite/utility/log.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {

			gl_shader::~gl_shader(){
				glDeleteProgram(m_shader);
			}

			void gl_shader::bind() {
				glUseProgram(m_shader);
			}

			gl_shader::gl_shader(const shader_create_args& args) {
				if (args.vertex_source.empty()) {
					LOG_ERROR("No vertex source given");
					return;
				}

				if (args.fragment_source.empty()) {
					LOG_ERROR("No fragment source given");
					return;
				}

				const size_t logSize = 512;
				int success = 0;
				char infoLog[logSize];

				// VERTEX
				unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
				const char* c_str = args.vertex_source.c_str();
				glShaderSource(vertex, 1, &c_str, NULL);
				glCompileShader(vertex);

				glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(vertex, logSize, NULL, infoLog);
					LOG_ERROR("Failed to compile vertex shader:\n{0}", infoLog);
					return;
				};

				// FRAGMENT
				unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
				c_str = args.fragment_source.c_str();
				glShaderSource(fragment, 1, &c_str, NULL);
				glCompileShader(fragment);

				glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
				if (!success) {
					glGetShaderInfoLog(fragment, logSize, NULL, infoLog);
					LOG_ERROR("Failed to compile fragment shader:\n{0}", infoLog);
					glDeleteShader(vertex);
					return;
				};

				// Create program
				m_shader = glCreateProgram();
				glAttachShader(m_shader, vertex);
				glAttachShader(m_shader, fragment);
				glLinkProgram(m_shader);

				glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
				if (!success) {
					glGetProgramInfoLog(m_shader, logSize, NULL, infoLog);
					LOG_ERROR("Failed to link shader:\n{0}", infoLog);
				}

				glDeleteShader(vertex);
				glDeleteShader(fragment);
			}

		}
	}
}
