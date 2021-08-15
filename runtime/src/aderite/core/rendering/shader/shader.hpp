#pragma once

#include <string>

namespace aderite {

	/**
	 * @brief Shader creation arguments
	*/
	struct shader_create_args {
		std::string vertex_source;
		std::string fragment_source;
	};

	/**
	 * @brief API agnostic shader object
	*/
	class shader {
	public:
		virtual ~shader() {}

		/**
		 * @brief Bind shader for use
		*/
		virtual void bind() = 0;

		/**
		 * @brief Creates a new API dependent shader object
		 * @param args Arguments of creation
		 * @return shader instance
		*/
		static shader* create(const shader_create_args& args);
	private:

	};

}
