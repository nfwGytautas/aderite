#include "PhysXActor.hpp"

#include <physx/PxRigidDynamic.h>
#include <physx/PxRigidStatic.h>
#include <physx/PxScene.h>
#include <physx/PxShape.h>

#include "aderite/Aderite.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/geometry/Geometry.hpp"
#include "aderite/scene/GameObject.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace physics {

PhysXActor::PhysXActor(scene::GameObject* gObject) : m_gObject(gObject) {}

PhysXActor::~PhysXActor() {
    // Delete all attached geometry
    this->freeGeometry();

    // Detach from scene
    this->freeActor();
}

void PhysXActor::makeStatic() {
    // Create actor instance
    physx::PxRigidStatic* actor =
        ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0)));
    actor->userData = this;

    // If this already has an actor, transfer colliders and position
    if (m_actor != nullptr) {
        this->transferGeometry(actor);
        actor->setGlobalPose(m_actor->getGlobalPose());
        this->freeActor();
    }

    m_actor = actor;
    m_isDynamic = false;
    m_gObject->getScene()->addActor(this);
}

void PhysXActor::makeDynamic() {
    // Create actor instance
    physx::PxRigidDynamic* actor =
        ::aderite::Engine::getPhysicsController()->getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0)));
    actor->userData = this;

    // If this already has an actor, transfer colliders
    if (m_actor != nullptr) {
        this->transferGeometry(actor);
        actor->setGlobalPose(m_actor->getGlobalPose());
        this->freeActor();
    }

    m_actor = actor;
    m_isDynamic = true;
    m_gObject->getScene()->addActor(this);
}

bool PhysXActor::isDynamic() const {
    return m_isDynamic;
}

void PhysXActor::applyProperties(const PhysicsProperties& properties) {
    if (m_actor == nullptr || m_isDynamic != properties.IsDynamic) {
        if (properties.IsDynamic) {
            this->makeDynamic();
        } else {
            this->makeStatic();
        }
    }

    // Apply properties
    if (m_isDynamic) {
        physx::PxRigidDynamic* dynamic = static_cast<physx::PxRigidDynamic*>(m_actor);
        dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !properties.HasGravity);
        dynamic->setMass(properties.Mass);
    }
}

PhysicsProperties PhysXActor::getProperties() const {
    PhysicsProperties pp;

    pp.IsDynamic = m_isDynamic;

    if (m_isDynamic) {
        physx::PxRigidDynamic* dynamic = static_cast<physx::PxRigidDynamic*>(m_actor);
        pp.HasGravity = !(dynamic->getActorFlags() & physx::PxActorFlag::eDISABLE_GRAVITY);
        pp.Mass = dynamic->getMass();
    }

    return pp;
}

void PhysXActor::addGeometry(Geometry* geometry) {
    ADERITE_DYNAMIC_ASSERT(geometry->getShape() != nullptr, "Nullptr shape of geometry");
    ADERITE_DYNAMIC_ASSERT(geometry->getShape()->getActor() == nullptr, "Already attached geometry being attached again");
    geometry->applyScale(m_scale);
    m_actor->attachShape(*geometry->getShape());
}

void PhysXActor::removeGeometry(Geometry* geometry) {
    m_actor->detachShape(*geometry->getShape());
    delete geometry;
}

std::vector<Geometry*> PhysXActor::getAttachedGeometries() const {
    std::vector<Geometry*> result;
    std::vector<physx::PxShape*> shapes;
    size_t count = m_actor->getNbShapes();

    // Resize and fill buffers
    result.reserve(count);
    shapes.resize(count);
    m_actor->getShapes(shapes.data(), count);

    std::transform(std::begin(shapes), std::end(shapes), std::back_inserter(result), [](physx::PxShape* s) {
        return static_cast<Geometry*>(s->userData);
    });

    return result;
}

void PhysXActor::freeGeometry() {
    for (Geometry* geometry : this->getAttachedGeometries()) {
        delete geometry;
    }
}

void PhysXActor::transferGeometry(physx::PxRigidActor* actor) {
    size_t count = m_actor->getNbShapes();
    std::vector<physx::PxShape*> shapes;
    shapes.resize(count);
    m_actor->getShapes(shapes.data(), count);

    for (physx::PxShape* shape : shapes) {
        actor->attachShape(*shape);
        m_actor->detachShape(*shape);
    }
}

void PhysXActor::freeActor() {
    physx::PxScene* scene = m_actor->getScene();

    if (scene != nullptr) {
        scene->removeActor(*m_actor);
    }
    m_actor->userData = nullptr;
    m_actor = nullptr;
}

physx::PxRigidActor* PhysXActor::getActor() const {
    return m_actor;
}

const glm::vec3& PhysXActor::getPosition() const {
    physx::PxTransform pxt = m_actor->getGlobalPose();
    return {pxt.p.x, pxt.p.y, pxt.p.z};
}

const glm::quat& PhysXActor::getRotation() const {
    physx::PxTransform pxt = m_actor->getGlobalPose();
    return {pxt.q.w, pxt.q.x, pxt.q.y, pxt.q.z};
}

const glm::vec3& PhysXActor::getScale() const {
    return m_scale;
}

void PhysXActor::setPosition(const glm::vec3& position) {
    physx::PxTransform pxt = m_actor->getGlobalPose();
    pxt.p = {position.x, position.y, position.z};
    m_actor->setGlobalPose(pxt);
}

void PhysXActor::setRotation(const glm::quat& rotation) {
    physx::PxTransform pxt = m_actor->getGlobalPose();
    pxt.q = {rotation.x, rotation.y, rotation.z, rotation.w};
    m_actor->setGlobalPose(pxt);
}

void PhysXActor::setScale(const glm::vec3& scale) {
    m_scale = scale;

    // Regenerate shapes
    for (physics::Geometry* geometry : this->getAttachedGeometries()) {
        geometry->applyScale(scale);
    }
}

bool PhysXActor::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "PhysicsActor" << YAML::BeginMap;

    // Transform
    if (!TransformProvider::serialize(serializer, emitter)) {
        return false;
    }

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

bool PhysXActor::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& actorNode = data["PhysicsActor"];
    if (!actorNode || actorNode.IsNull()) {
        return false;
    }

    // Transform
    if (!TransformProvider::deserialize(serializer, actorNode)) {
        return false;
    }

    // Geometry
    for (const YAML::Node& geometryNode : actorNode["Geometry"]) {
        Geometry* geom = static_cast<Geometry*>(serializer->parseObject(geometryNode));
        this->addGeometry(geom);
    }

    return true;
}

} // namespace physics
} // namespace aderite
