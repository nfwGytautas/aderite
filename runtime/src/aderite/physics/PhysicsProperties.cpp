#include "PhysicsProperties.hpp"

#include <PxRigidActor.h>

#include "aderite/io/Serializer.hpp"
#include "aderite/physics/PhysXActor.hpp"
#include "aderite/physics/geometry/Geometry.hpp"

namespace aderite {
namespace physics {

PhysicsProperties::~PhysicsProperties() {
    for (Geometry* geometry : this->getAttachedGeometries()) {
        delete geometry;
    }
}

void PhysicsProperties::addGeometry(Geometry* geometry) {
    m_geometryChanged = true;
    m_geometry.push_back(geometry);
}

void PhysicsProperties::removeGeometry(Geometry* geometry) {
    m_geometryChanged = true;
    m_geometry.erase(std::remove(m_geometry.begin(), m_geometry.end(), geometry), m_geometry.end());
    delete geometry;
}

std::vector<Geometry*> PhysicsProperties::getAttachedGeometries() const {
    return m_geometry;
}

bool PhysicsProperties::hasGeometryChanged() const {
    return m_geometryChanged;
}

void PhysicsProperties::resetGeometryChangedFlag() {
    m_geometryChanged = false;
}

bool PhysicsProperties::isDynamic() const {
    return m_dynamic;
}

void PhysicsProperties::makeStatic() {
    m_dynamic = false;
}

void PhysicsProperties::makeDynamic() {
    m_dynamic = true;
}

bool PhysicsProperties::hasGravity() const {
    return m_hasGravity;
}

void PhysicsProperties::disableGravity() {
    m_hasGravity = false;
}

void PhysicsProperties::enableGravity() {
    m_hasGravity = true;
}

float PhysicsProperties::getMass() const {
    return m_mass;
}

void PhysicsProperties::setMass(float mass) {
    m_mass = mass;
}

bool PhysicsProperties::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    emitter << YAML::Key << "PhysicsActor" << YAML::BeginMap;

    emitter << YAML::Key << "Dynamic" << YAML::Value << m_dynamic;
    emitter << YAML::Key << "Mass" << YAML::Value << m_mass;
    emitter << YAML::Key << "HasGravity" << YAML::Value << m_hasGravity;

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

bool PhysicsProperties::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    const YAML::Node& actorNode = data["PhysicsActor"];
    if (!actorNode || actorNode.IsNull()) {
        return false;
    }

    m_dynamic = actorNode["Dynamic"].as<bool>();
    m_mass = actorNode["Mass"].as<float>();
    m_hasGravity = actorNode["HasGravity"].as<bool>();

    // Geometry
    for (const YAML::Node& geometryNode : actorNode["Geometry"]) {
        Geometry* geom = static_cast<Geometry*>(serializer->parseObject(geometryNode));
        this->addGeometry(geom);
    }

    return true;
}

PhysicsProperties& PhysicsProperties::operator=(const PhysicsProperties& other) {
    m_dynamic = other.m_dynamic;
    m_hasGravity = other.m_hasGravity;
    m_mass = other.m_mass;

    // Copy geometry
    m_geometryChanged = true;
    m_geometry.reserve(other.m_geometry.size());
    for (Geometry* geom : other.m_geometry) {
        m_geometry.push_back(geom->clone());
    }

    return *this;
}

} // namespace physics
} // namespace aderite
