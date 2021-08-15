#pragma once

#include <filesystem>

namespace aderite {
	namespace editor {
		namespace utility {

			/**
			 * @brief Creates a unique path from the specified path, this is done by checking if a file
			 * already exists if yes than a number is appended before the extension until there doesn't exist
			 * a file with the same name
			 * @param path Wanted filepath
			 * @return Unique filepath
			*/
			std::filesystem::path make_unique_path(const std::filesystem::path& path);

		}
	}
}
