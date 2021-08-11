#include "utility.hpp"

namespace aderite {
	namespace editor {
		namespace utility {
			std::filesystem::path make_unique_path(const std::filesystem::path& path) {
				const std::string filename = path.stem().string();
				const std::string extension = path.extension().string();
				const std::filesystem::path root = path.parent_path();

				std::filesystem::path path_name = root / (filename + extension);
				int it = 1;
				while (std::filesystem::exists(path_name)) {
					path_name = root / (filename + " " + std::to_string(it++) + extension);
				}

				return path_name;
			}
		}
	}
}
