#pragma once

#include <string>
#include <vector>
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Dialog for selecting files, directories, etc. Currently uses PFD
*/
class FileDialog {
public:
	/**
	 * @brief Opens a select directory dialog and returns selected directory
	 * @return Directory path
	*/
	static std::string selectDirectory();

	/**
	 * @brief Select file with an optional filter
	 * @param title Title of the dialog
	 * @param filter Vector of filter to apply for example {"Aderite project", "*.aproj"}
	 * @return Path to selected file
	*/
	static std::string selectFile(const std::string& title, const std::vector<std::string>& filter = {"All files", "*"});
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
