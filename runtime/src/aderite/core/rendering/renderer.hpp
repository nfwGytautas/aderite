#pragma once

#include <vector>

#include "aderite/interfaces/iframe_object.hpp"
#include "aderite/core/rendering/layer.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/core/rendering/fbo/fbo.hpp"

namespace aderite {

	/**
	 * @brief Generic rendering class, automatically configures for a backend
	*/
	class renderer : public interfaces::iframe_object {
	public:
		virtual ~renderer() {}

		/**
		 * @brief Initializes the renderer
		*/
		virtual bool init() = 0;

		/**
		 * @brief Shutdown renderer
		*/
		virtual void shutdown();

		/**
		 * @brief Enables or disables vsync
		*/
		virtual void set_vsync(bool enabled) = 0;

		/**
		 * @brief Adds a new layer to the renderer
		 * @tparam T Layer to add
		*/
		template<typename T, typename ...Args>
		void add_layer(Args&&... args) {
			if (std::is_base_of<layer, T>::value) {
				ref<layer> l = ref<layer>(new T(std::forward<Args>(args)...));
				m_layers.push_back(l);
				l->i_init();
			}
			else {
				LOG_ERROR("Tried to add a layer that doesn't inherit layer object");
			}
		}

		/**
		 * @brief Executes a single render pass
		*/
		void render();

		/**
		 * @brief Sets the render target, if nullptr then the active window will be the target
		 * @param target New render target
		*/
		virtual void set_output(relay_ptr<fbo> target) = 0;

		/**
		 * @brief Returns true if renderer ready to render
		*/
		virtual bool ready() = 0;
	private:
		/**
		 * @brief Creates a instance of the renderer depending on the rendering backend
		*/
		static renderer* create_instance();

	protected:
		renderer() {}
		friend class engine;

		std::vector<ref<layer>> m_layers;
	};

}
