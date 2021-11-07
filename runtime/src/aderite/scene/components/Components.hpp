#pragma once

/**
 * @brief All components supported natively by aderite are described in this file
 */


#include <glm/glm.hpp>

#include "aderite/Handles.hpp"
#include "aderite/asset/Forward.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/scripting/Forward.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scene {

/**
 * @brief Camera component, which references ICamera object
 */
struct CameraComponent {
    bool Main = true;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

/**
 * @brief Component used to specify that this entity has ScriptableBehaviors applied to it
 */
struct ScriptsComponent {
    scripting::ScriptList* Scripts = nullptr;

    ScriptsComponent() = default;
    ScriptsComponent(const ScriptsComponent&) = default;
};

} // namespace scene
} // namespace aderite
