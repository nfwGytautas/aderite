#pragma once

#include <vector>

#include "aderite/interfaces/iframe_object.hpp"
#include "aderite/core/rendering/layer.hpp"
#include "aderite/utility/log.hpp"
#include "aderite/utility/pointer.hpp"

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
		template<typename T>
		void add_layer() {
			if (std::is_base_of<layer, T>::value) {
				ref<layer> l = ref<layer>(new T());
				m_layers.push_back(l);
				l->i_init();
			}
			else {
				LOG_ERROR("Tried to add a layer that doesn't inherit layer object");
			}
		}

		/**
		 * @brief Executes a single render cycle
		*/
		void render();
	private:
		/**
		 * @brief Creates a instance of the renderer depending on the rendering backend
		 * @return 
		*/
		static renderer* create_instance();

	protected:
		renderer() {}
		friend class engine;

		std::vector<ref<layer>> m_layers;
	};

}
