#pragma once

namespace aderite {

	/**
	 * @brief Framebuffer creation arguments
	*/
	struct fbo_create_args {
		unsigned int width;
		unsigned int height;
	};

	/**
	 * @brief API agnostic framebuffer object
	*/
	class fbo {
	public:
		virtual ~fbo() {};

		/**
		 * @brief Bind the framebuffer
		*/
		virtual void bind() = 0;

		/**
		 * @brief Returns the texture attachment object
		*/
		virtual void* get_ta() = 0;

		/**
		 * @brief Creates a new API dependent framebuffer object
		 * @param args Arguments of creation
		 * @return fbo instance
		*/
		static fbo* create(const fbo_create_args& args);

	protected:
		unsigned int width;
		unsigned int height;
	};

}
