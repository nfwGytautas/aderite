#include "EntityProvideOperation.hpp"
#include <functional>

#include <bgfx/bgfx.h>

#include "aderite/Aderite.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/rendering/Renderable.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Utility.hpp"

namespace aderite {
namespace rendering {

void EntityProvideOperation::execute(PipelineState* state) {
    // Get current scene
    scene::Scene* currentScene = ::aderite::Engine::getSceneManager()->getCurrentScene();
    if (!currentScene) {
        m_lookup.clear();
        m_drawcalls.clear();
        return;
    }

    // Clean transformations from previous call
    cleanList();

    // Entity group
    for (scene::Entity* entity : currentScene->getEntities()) {
        Renderable* renderable = entity->getRenderable();
        if (renderable != nullptr) {
            if (renderable->isValid()) {
                // Check if draw call already exists
                size_t hash = utility::combineHash(renderable->getMesh()->getHandle(), renderable->getMaterial()->getHandle());

                if (m_lookup.find(hash) == m_lookup.end()) {
                    // Create new drawcall and insert
                    m_drawcalls.insert(m_drawcalls.end(), renderable->createDrawCall());
                    m_lookup[hash] = m_drawcalls.size() - 1;
                }

                m_drawcalls[m_lookup[hash]].Transformations.push_back(entity->getTransform()->computeTransform());
            } else {
                renderable->loadIfNeeded();
            }
        }
    }

    state->setDrawCallList(&m_drawcalls);
}

void EntityProvideOperation::cleanList() {
    m_drawcalls.clear();
    m_lookup.clear();
}

reflection::Type EntityProvideOperation::getType() const {
    return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_ENTITY);
}

bool EntityProvideOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) const {
    return true;
}

bool EntityProvideOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
    return true;
}

} // namespace rendering
} // namespace aderite
