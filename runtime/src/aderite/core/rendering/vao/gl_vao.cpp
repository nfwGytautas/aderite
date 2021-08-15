#include "gl_vao.hpp"

#include <glad/glad.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/rendering/vao/vao_layout.hpp"

namespace aderite {
	namespace render_backend {
		namespace opengl {
			gl_vao::gl_vao() {
				ASSERT_RENDER_THREAD;
				glGenVertexArrays(1, &m_vao);
			}

			gl_vao::~gl_vao() {
				ASSERT_RENDER_THREAD;
				glDeleteVertexArrays(1, &m_vao);

				if (m_positionVbo != 0) {
					glDeleteBuffers(1, &m_positionVbo);
				}
			}
			
			void gl_vao::bind() {
				ASSERT_RENDER_THREAD;
				glBindVertexArray(m_vao);
			}
			
			void gl_vao::change_layout(const vao_layout& layout) {
				ASSERT_RENDER_THREAD;
				bind();
				
				// Position
				if (layout.HasPosition) {
					if (layout.PositionCount == 0 || layout.PositionCount % 3 != 0) {
						LOG_WARN("Possibly incorrect data submitted for VAO position buffer {0}, make sure it's not 0 and is divisible by {1}", layout.PositionCount, 3);
					}

					// Create buffer
					glGenBuffers(1, &m_positionVbo);
					glBindBuffer(GL_ARRAY_BUFFER, m_positionVbo);
					glBufferData(GL_ARRAY_BUFFER, sizeof(float) * layout.PositionCount, layout.PositionData, layout.IsPositionStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
					
					// Setup attributes
					glVertexAttribPointer(layout.PositionStart, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)layout.PositionStart);
					glEnableVertexAttribArray(layout.PositionStart);

					m_count = layout.PositionCount;
				}

				// Indices
				if (layout.HasIndices) {
					// Create buffer
					glGenBuffers(1, &m_ebo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * layout.IndicesCount, layout.IndicesData, GL_STATIC_DRAW);

					m_count = layout.IndicesCount;
				}

				// Just in case
				glBindVertexArray(0);
			}

			void gl_vao::render() {
				ASSERT_RENDER_THREAD;
				if (m_ebo != 0) {
					glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, 0);
				} 
				else {
					glDrawArrays(GL_TRIANGLES, 0, m_count);
				}
			}
		}
	}
}