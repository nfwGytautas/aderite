#include "Renderable.hpp"

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/rendering/Renderer.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/TransformProvider.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace rendering {

inline glm::mat4 calculateTransformationMatrix(scene::TransformProvider* transform) {
    glm::vec3 position = transform->getPosition();
    glm::quat rotation = transform->getRotation();
    glm::vec3 scale = transform->getScale();

    glm::mat4 rMat = glm::toMat4(rotation);
    return glm::translate(glm::mat4(1.0f), position) * rMat * glm::scale(glm::mat4(1.0f), scale);
}

Renderable::Renderable(scene::GameObject* gObject) : m_gObject(gObject) {}

Renderable::~Renderable() {}

void Renderable::update(float delta) {
    if (!m_data.isValid()) {
        return;
    }

    scene::TransformProvider* const transform = m_gObject->getTransform();

    if (transform == nullptr) {
        return;
    }

    rendering::FrameData& fd = ::aderite::Engine::getRenderer()->getWriteFrameData();

    rendering::DrawCall& dc = fd.DrawCalls[m_data.hash()];
    dc.Renderable = this;
    dc.Transformations.push_back(calculateTransformationMatrix(transform));
}

RenderableData& Renderable::getData() {
    return m_data;
}

} // namespace rendering
} // namespace aderite
