#pragma once

namespace aderite {
	class vao_layout;

	/**
	 * @brief Interface for a API agnostic VAO object
	*/
	class vao {
	public:
		virtual ~vao() {}

		/**
		 * @brief Bind the VAO for use
		*/
		virtual void bind() = 0;

		/**
		 * @brief Changes internal VAO layout to the one specified
		 * @param layout Layout
		*/
		virtual void change_layout(const vao_layout& layout) = 0;

		/**
		 * @brief Render the VAO
		*/
		virtual void render() = 0;

		/**
		 * @brief Creates a new API dependent vao object
		 * @return shader instance
		*/
		static vao* create();
	};

}
