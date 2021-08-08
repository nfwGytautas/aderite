#pragma once

#include <filesystem>
#include "aderite_editor/components/base_component.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Asset browser used by aderite. This component allows the user to browse the root asset manager directory.
			*/
			class asset_browser : public base_component {
			public:
				enum class fs_node_type {
					UNKNOWN = -1,
					DIRECTORY = 0,
					SCENE = 1,
				};

				struct fs_node {
					fs_node_type Type;
					std::string Stem;
					std::string Name;
				};

			public:
				asset_browser();
				virtual ~asset_browser();

				// Inherited via base_component
				virtual void render() override;

			private:
				/**
				 * @brief Resolve the directories depending on the current dir
				*/
				void resolve_fs();

			private:
				// How many frames to wait before updating
				int m_update_interval = 15;

				// This is relative to root directory
				std::filesystem::path m_current_dir;

				std::vector<fs_node> m_it_nodes = {};
				std::vector<std::string> m_path_nodes = {};
			};

		}
	}
}