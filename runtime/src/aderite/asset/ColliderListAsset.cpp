#include "ColliderListAsset.hpp"

#include "aderite/io/Serializer.hpp"
#include "aderite/physics/Collider.hpp"

namespace aderite {
namespace asset {

ColliderListAsset::~ColliderListAsset() {
    for (physics::Collider* c : m_colliders) {
        delete c;
    }
}

void ColliderListAsset::attachTo(physics::PhysicsActor* actor, const glm::vec3& globalScale) {
    for (physics::Collider* c : m_colliders) {
        c->attach(actor, globalScale);
    }
}

void ColliderListAsset::add(physics::Collider* collider) {
    m_colliders.push_back(collider);
    this->incrementIteration();
}

void ColliderListAsset::remove(physics::Collider* collider) {
    auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);

    if (it == m_colliders.end()) {
        return;
    }

    m_colliders.erase(it);
    this->incrementIteration();
}

size_t ColliderListAsset::getIteration() const {
    return m_iteration;
}

void ColliderListAsset::incrementIteration() {
    m_iteration++;
}

reflection::Type ColliderListAsset::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::CLDR_LIST);
}

bool ColliderListAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
    // Output to file
    emitter << YAML::Key << "Colliders";
    emitter << YAML::BeginSeq;
    for (physics::Collider* c : m_colliders) {
        serializer->writeUntrackedType(emitter, c);
    }
    emitter << YAML::EndSeq;

    return true;
}

bool ColliderListAsset::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    for (auto colliderEntry : data["Colliders"]) {
        YAML::Node& colliderNode = colliderEntry;
        physics::Collider* collider = static_cast<physics::Collider*>(serializer->parseUntrackedType(colliderEntry));
        this->add(collider);
    }

    return true;
}

} // namespace asset
} // namespace aderite
