#pragma once

#include <string>
#include <vector>

namespace aderite {
	namespace editor {

		/**
		 * @brief Dialog for selecting files, directories, etc. Currently uses PFD
		*/
		class file_dialog {
		public:
			/**
			 * @brief Opens a select directory dialog and returns selected directory
			 * @return Directory path
			*/
			static std::string select_directory();

			/**
			 * @brief Select file with an optional filter
			 * @param title Title of the dialog
			 * @param filter Vector of filter to apply for example {"Aderite project", "*.aproj"}
			 * @return Path to selected file
			*/
			static std::string select_file(const std::string& title, const std::vector<std::string>& filter = {"All files", "*"});
		};

	}
}
