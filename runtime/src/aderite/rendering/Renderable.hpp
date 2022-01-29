#pragma once

#include "aderite/io/SerializableObject.hpp"
#include "aderite/rendering/RenderableData.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace rendering {

/**
 * @brief A class that provides functionality for renderable objects
 */
class Renderable {
public:
    Renderable(scene::GameObject* gObject);
    virtual ~Renderable();

    /**
     * @brief Update the renderable properties
     * @param delta Delta time of last frame
     */
    void update(float delta);

    /**
     * @brief Returns the renderable data of this renderable
     */
    RenderableData& getData();

private:
    scene::GameObject* m_gObject = nullptr;
    RenderableData m_data;
};

} // namespace rendering
} // namespace aderite
