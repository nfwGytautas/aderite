#pragma once

#include "aderite/Aderite.hpp"
#include "aderite/reflection/Reflector.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

#include "aderiteeditor/asset/EditorMaterialType.hpp"

namespace aderite {
namespace utility {

/**
 * @brief Utility function to link instancers
 */
void linkInstancers() {
    aderite::reflection::Reflector* reflector = aderite::Engine::getReflector();

    ADERITE_REFLECTOR_EXPOSE_INSTANCE(reflector, asset::EditorMaterialType, reflection::RuntimeTypes::MAT_TYPE);
}

} // namespace utility
} // namespace aderite
