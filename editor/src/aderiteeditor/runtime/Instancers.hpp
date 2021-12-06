#pragma once

#include "aderite/Aderite.hpp"
#include "aderite/reflection/Reflector.hpp"

#include "aderiteeditor/runtime/EditorTypes.hpp"

namespace aderite {
namespace utility {

/**
 * @brief Utility function to link instancers
 */
void linkInstancers() {
    aderite::reflection::Reflector* reflector = aderite::Engine::getReflector();

}

} // namespace utility
} // namespace aderite
