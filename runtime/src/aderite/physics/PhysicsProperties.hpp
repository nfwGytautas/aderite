#pragma once
#include <vector>

#include "aderite/io/ISerializable.hpp"
#include "aderite/physics/Forward.hpp"

namespace aderite {
namespace physics {

/**
 * @brief Properties that can be applied to a physics actor
 */
class PhysicsProperties final : public io::ISerializable {
public:
    virtual ~PhysicsProperties();

    /**
     * @brief Add geometry
     * @param geometry Geometry instance
     */
    void addGeometry(Geometry* geometry);

    /**
     * @brief Remove geometry
     * @param geometry Geometry to remove
     */
    void removeGeometry(Geometry* geometry);

    /**
     * @brief Returns a list of attached geometries
     */
    std::vector<Geometry*> getAttachedGeometries() const;

    /**
     * @brief Returns true if geometry was changed
     */
    bool hasGeometryChanged() const;

    /**
     * @brief Resets the geometry change flag
     */
    void resetGeometryChangedFlag();

    /**
     * @brief Returns true if the actor should be dynamic
     */
    bool isDynamic() const;

    /**
     * @brief Make actor dynamic
     */
    void makeStatic();

    /**
     * @brief Make actor static
     */
    void makeDynamic();

    /**
     * @brief Returns true if the actor has gravity, false otherwise, always false for static actors
     */
    bool hasGravity() const;

    /**
     * @brief Disable gravity
     */
    void disableGravity();

    /**
     * @brief Enable gravity
     */
    void enableGravity();

    /**
     * @brief Returns the mass of the actor
     */
    float getMass() const;

    /**
     * @brief Set the mass of the actor
     * @param mass New mass of the actor
     */
    void setMass(float mass);

    // Inherited via ISerializable
    bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const override;
    bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

    PhysicsProperties& operator=(const PhysicsProperties& other);

private:
    bool m_dynamic = false;

    bool m_geometryChanged = false;
    std::vector<physics::Geometry*> m_geometry;

    // Dynamic only

    /**
     * @brief If true the actor is affected by gravity
     */
    bool m_hasGravity = false;

    /**
     * @brief Mass of the object
     */
    float m_mass = 1.0f;
};

} // namespace physics
} // namespace aderite
