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
					SHADER = 2,
					MESH = 3,
					MATERIAL = 4
				};

				struct fs_node {
					fs_node_type Type;
					std::string Stem;
					std::string Name;
					std::string Extension;
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

				/**
				 * @brief Deletes an item from the filesystem, be it directory or a simple file
				 * @param path Path to object delete
				*/
				void delete_item(std::filesystem::path path);

				/**
				 * @brief Renames an item and updates asset manager registry
				 * @param prevName Previous name of the object
				 * @param newName New name of the object
				*/
				void rename_item(const std::string& prevName, const std::string& newName);

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