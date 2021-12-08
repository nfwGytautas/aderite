#include "Reflector.hpp"

#include "aderite/reflection/Instancer.hpp"
#include "aderite/reflection/Reflectable.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"

// Assets, needed for linking instancers
#include "aderite/asset/AudioAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/audio/AudioListener.hpp"
#include "aderite/audio/AudioSource.hpp"
#include "aderite/physics/geometry/BoxGeometry.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/DynamicPhysicsRegion.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/Scenery.hpp"
#include "aderite/scene/StaticPhysicsRegion.hpp"
#include "aderite/scene/Visual.hpp"
#include "aderite/scripting/ScriptSystem.hpp"

namespace aderite {
namespace reflection {

bool Reflector::init() {
    ADERITE_LOG_BLOCK;
    // Runtime instancers
    LOG_TRACE("[Reflection] Setting runtime instancers");

    // Assets
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MeshAsset, RuntimeTypes::MESH);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MaterialAsset, RuntimeTypes::MATERIAL);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::TextureAsset, RuntimeTypes::TEXTURE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::Scene, RuntimeTypes::SCENE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::MaterialTypeAsset, RuntimeTypes::MAT_TYPE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, asset::AudioAsset, RuntimeTypes::AUDIO);

    // Audio
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, audio::AudioSource, RuntimeTypes::AUDIO_SOURCE);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, audio::AudioListener, RuntimeTypes::AUDIO_LISTENER);

    // Object
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::Visual, RuntimeTypes::VISUAL);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::Scenery, RuntimeTypes::SCENERY);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::Entity, RuntimeTypes::ENTITY);

    // Physics
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::StaticPhysicsRegion, RuntimeTypes::STATIC_PHYSICS_REGION);
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, scene::DynamicPhysicsRegion, RuntimeTypes::DYNAMIC_PHYSICS_REGION);

    // Geometry
    ADERITE_REFLECTOR_EXPOSE_INSTANCE(this, physics::BoxGeometry, RuntimeTypes::BOX_GEOMETRY);

    LOG_DEBUG("[Reflection] Registered {0} runtime instancers", m_instancers.size());
    ADERITE_DEBUG_SECTION(this->printInstancers(););

    LOG_INFO("[Reflection] Reflector initialized");

    return true;
}

void Reflector::shutdown() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[Reflection] Shutting down reflector");
    for (auto instancer : m_instancers) {
        delete instancer.second;
    }
    LOG_INFO("[Reflection] Reflector shutdown");
}

void Reflector::linkInstancer(Type type, InstancerBase* instancer, const std::string& name = "") {
    auto it = m_instancers.find(type);
    if (it != m_instancers.end()) {
        // Already has mapped instancer
        LOG_DEBUG("[Reflection] Instancer for type {0:03d}({1}) has been overridden to ({2})", type, m_names[type], name);

        delete it->second;
        it->second = instancer;

        return;
    } else {
        LOG_TRACE("[Reflection] Registered instancer for type {0:03d}({1})", type, name);
    }

    m_instancers.insert_or_assign(type, instancer);
    m_names.insert_or_assign(type, name);
}

std::string Reflector::reflectName(Type type) const {
    return m_names.at(type);
}

InstancerBase* Reflector::resolveInstancer(Type type) const {
    auto it = m_instancers.find(type);
    ADERITE_DYNAMIC_ASSERT(it != m_instancers.end(), "Tried to resolve instancer for non registered type");
    return it->second;
}

ADERITE_DEBUG_SECTION(void Reflector::printInstancers() {
    LOG_TRACE("");
    LOG_TRACE("====================================================================================");
    LOG_TRACE("                                    INSTANCERS                                      ");
    LOG_TRACE("====================================================================================");
    for (auto i : m_instancers) {
        if (i.second != nullptr) {
            Reflectable* temp = nullptr;
            temp = i.second->create();
            LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: {2:03d} Name: {3}", i.first, static_cast<void*>(i.second),
                      temp->getType(), m_names[temp->getType()]);
            delete temp;
        } else {
            LOG_TRACE("Type: {0:03d} Instancer: {1:p} Created type: UNKNOWN", i.first, static_cast<void*>(i.second));
        }
    }
    LOG_TRACE("");
})

} // namespace reflection
} // namespace aderite
