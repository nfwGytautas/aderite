#pragma once

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
	};

}
