#include "renderer.hpp"

#include "aderite/config.hpp"

#if GL_BACKEND
#include "aderite/core/rendering/gl_renderer.hpp"
#endif

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/core/window/window.hpp"
#include "aderite/core/assets/object/mesh_asset.hpp"
#include "aderite/core/assets/object/material_asset.hpp"
#include "aderite/core/assets/object/shader_asset.hpp"
#include "aderite/core/scene/scene_manager.hpp"
#include "aderite/core/scene/components.hpp"
#include "aderite/core/scene/scene.hpp"
#include "aderite/core/rendering/fbo/fbo.hpp"
#include "aderite/core/rendering/vao/vao.hpp"
#include "aderite/core/rendering/shader/shader.hpp"

namespace aderite {

	renderer* renderer::create_instance() {
		LOG_TRACE("Creating renderer instance");
#if GL_BACKEND
		return new render_backend::opengl::gl_renderer();
#endif

		LOG_ERROR("No rendering backend specified");
		return nullptr;
	}

	bool renderer::init(window* wnd) {
		// Setup default render target
		m_attached_to = wnd;
		return true;
	}

	void renderer::shutdown() {

	}

	void renderer::render() {
		if (!ready()) {
			return;
		}

		// TODO: Sorting, batching, output target
		set_output(nullptr);
		
		// TODO: Camera, position, perspective matrix

		// Render entities
		scene::scene* scene = engine::get_scene_manager()->current_scene();
		auto group = scene->get_entity_registry().group<scene::components::transform>(entt::get<scene::components::mesh_renderer>);
		for (auto entity : group) {
			// TODO: Layers
			auto [transform, mr] = group.get<scene::components::transform, scene::components::mesh_renderer>(entity);

			if (mr.MeshHandle == nullptr || mr.MaterialHandle == nullptr) {
				continue;
			}

			glm::mat4 tmat = scene::components::transform::compute_transform(transform);

			vao* mesh = static_cast<asset::mesh_asset*>(mr.MeshHandle)->object();
			asset::material_asset* material = static_cast<asset::material_asset*>(mr.MaterialHandle);
			shader* shader = static_cast<asset::shader_asset*>(material->get_fields().Shader)->object();

			shader->bind();
			mesh->bind();
			mesh->render();
		}
	}

	void renderer::set_output(fbo* target) {
		if (!ready()) {
			return;
		}

		if (target != nullptr) {
			target->bind();
		}
		else {
			output_to_default();
		}
	}

	void renderer::set_default_target(fbo* target) {
		m_default_target = target;
	}

}