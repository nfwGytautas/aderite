#pragma once

/**
 * @brief This file is used to define forward declarations for all Scene types
 */

#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace scene {

class SceneManager;
class Scene;

class ITransformProvider;

// Objects
class Visual;
class Scenery;
class Entity;

// Physics
class StaticPhysicsRegion;
class DynamicPhysicsRegion;

class Camera;

} // namespace scene
} // namespace aderite
