#pragma once

#include "aderiteeditor/utility/Macros.hpp"

#include "aderite/asset/MaterialTypeAsset.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

/**
 * @brief Compiles material asset to the current raw directory
 * @param type Type to compile
*/
void compileMaterialType(asset::MaterialTypeAsset* type);

ADERITE_EDITOR_COMPILER_NAMESPACE_END
