#pragma once

#include <filesystem>
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_UTILITY_NAMESPACE_BEGIN

/**
 * @brief Creates a unique path from the specified path, this is done by checking if a file
 * already exists if yes than a number is appended before the extension until there doesn't exist
 * a file with the same name
 * @param path Wanted filepath
 * @return Unique filepath
*/
std::filesystem::path makeUniquePath(const std::filesystem::path& path);

ADERITE_EDITOR_UTILITY_NAMESPACE_END
