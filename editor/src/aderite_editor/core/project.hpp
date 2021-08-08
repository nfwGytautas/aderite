#pragma once

#include <string>
#include <filesystem>

namespace aderite {
	namespace editor {

		/**
		 * @brief Class containing all information about aderite project
		*/
		class project {
		public:
			/**
			 * @brief Creates a new project object with name and base directory
			*/
			project(const std::string& dir, const std::string& name);
			virtual ~project();

			/**
			 * @brief Save project to file
			 * @return True if saved without errors
			*/
			bool save();

			/**
			 * @brief Load project
			 * @param path The project file
			 * @return New project object if loaded successfully, nullptr otherwise
			*/
			static project* load(const std::string& path);

			/**
			 * @brief Returns the project name
			*/
			std::string get_name() const;

			/**
			 * @brief Returns the project root directory
			*/
			std::filesystem::path get_root_dir() const;

			/**
			 * @brief Returns the active scene of the project
			*/
			std::string get_active_scene() const;
		private:
			std::filesystem::path m_directory;
			std::string m_name;
			std::string m_active_scene;
		};
	}
}