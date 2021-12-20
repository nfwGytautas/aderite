#include "BasePhysicsActor.hpp"
#include <algorithm>
#include <iterator>

#include <physx/PxScene.h>
#include <physx/PxShape.h>

#include "aderite/io/Serializer.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/utility/YAML.hpp"

namespace aderite {
namespace physics {

BasePhysicsActor::~BasePhysicsActor() {
    // Delete all attached geometry
    this->freeGeometry();

    // Detach from scene
    physx::PxScene* scene = p_actor->getScene();

    if (scene != nullptr) {
        scene->removeActor(*p_actor);
    }
    p_actor->userData = nullptr;
}

void BasePhysicsActor::addGeometry(Geometry* geometry) {
    ADERITE_DYNAMIC_ASSERT(geometry->p_shape != nullptr, "Nullptr shape of geometry");
    ADERITE_DYNAMIC_ASSERT(geometry->p_shape->getActor() == nullptr, "Already attached geometry being attached again");
    geometry->applyScale(m_scale);
    p_actor->attachShape(*geometry->p_shape);
}

void BasePhysicsActor::removeGeometry(Geometry* geometry) {
    p_actor->detachShape(*geometry->p_shape);
    delete geometry;
}

std::vector<Geometry*> BasePhysicsActor::getAttachedGeometries() const {
    std::vector<Geometry*> result;
    std::vector<physx::PxShape*> shapes;
    size_t count = p_actor->getNbShapes();

    // Resize and fill buffers
    result.reserve(count);
    shapes.resize(count);
    p_actor->getShapes(shapes.data(), count);

    std::transform(std::begin(shapes), std::end(shapes), std::back_inserter(result), [](physx::PxShape* s) {
        return static_cast<Geometry*>(s->userData);
    });

    return result;
}

const glm::vec3& BasePhysicsActor::getPosition() const {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    return {pxt.p.x, pxt.p.y, pxt.p.z};
}

const glm::quat& BasePhysicsActor::getRotation() const {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    return {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};
}

const glm::vec3& BasePhysicsActor::getScale() const {
    return m_scale;
}

void BasePhysicsActor::setPosition(const glm::vec3& position) {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    pxt.p = {position.x, position.y, position.z};
    p_actor->setGlobalPose(pxt);
}

void BasePhysicsActor::setRotation(const glm::quat& rotation) {
    physx::PxTransform pxt = p_actor->getGlobalPose();
    pxt.q = {rotation.x, rotation.y, rotation.z, rotation.w};
    p_actor->setGlobalPose(pxt);
}

void BasePhysicsActor::setScale(const glm::vec3& scale) {
    m_scale = scale;

    // Regenerate shapes
    for (Geometry* geometry : this->getAttachedGeometries()) {
        geometry->applyScale(scale);
    }
}

void BasePhysicsActor::freeGeometry() {
    for (Geometry* geometry : this->getAttachedGeometries()) {
        delete geometry;
    }
}

BasePhysicsActor::BasePhysicsActor() {}

bool BasePhysicsActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "BasePhysicsActor" << YAML::BeginMap;

    // Transform
    glm::vec3 p = this->getPosition();
    glm::quat q = this->getRotation();

    emitter << YAML::Key << "Transform" << YAML::BeginMap;
    emitter << YAML::Key << "Position" << YAML::Value << p;
    emitter << YAML::Key << "Rotation" << YAML::Value << q;
    emitter << YAML::Key << "Scale" << YAML::Value << m_scale;
    emitter << YAML::EndMap;

    // Geometry
    emitter << YAML::Key << "Geometry";
    emitter << YAML::BeginSeq;
    for (Geometry* geom : this->getAttachedGeometries()) {
        serializer->writeObject(emitter, geom);
    }
    emitter << YAML::EndSeq;

    emitter << YAML::EndMap;

    return true;
}

bool BasePhysicsActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& actorNode = data["BasePhysicsActor"];
    if (!actorNode || actorNode.IsNull()) {
        return false;
    }

    // Transform
    const YAML::Node& transformNode = actorNode["Transform"];
    this->setPosition(transformNode["Position"].as<glm::vec3>());
    this->setRotation(transformNode["Rotation"].as<glm::quat>());
    this->setScale(transformNode["Scale"].as<glm::vec3>());

    // Geometry
    for (const YAML::Node& geometryNode : actorNode["Geometry"]) {
        Geometry* geom = static_cast<Geometry*>(serializer->parseObject(geometryNode));
        this->addGeometry(geom);
    }

    return true;
}

} // namespace physics
} // namespace aderite
