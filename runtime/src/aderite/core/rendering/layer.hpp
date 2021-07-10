#pragma once

#include "aderite/core/rendering/fbo/fbo.hpp"

namespace aderite {

	/**
	 * @brief Rendering layer for aderite
	*/
	class layer {
	public:
		virtual ~layer();

		/**
		 * @brief Initialize layer
		*/
		virtual void init() = 0;

		/**
		 * @brief Render layer
		*/
		virtual void render() = 0;

		/**
		 * @brief Shutdown layer
		*/
		virtual void shutdown() = 0;

		/**
		 * @brief Check if layer is ready to be rendered 
		*/
		virtual bool ready() = 0;

	private:
		void i_init();
		void i_shutdown();

	protected:
		layer();
		friend class renderer;

	protected:
		// The target where the renderer will render this layer to
		relay_ptr<fbo> target = nullptr;
	};

}
