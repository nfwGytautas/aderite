#include "EntityProvideOperation.hpp"

#include <functional>
#include <bgfx/bgfx.h>
#include "aderite/Aderite.hpp"
#include "aderite/utility/Utility.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/io/LoaderPool.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

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
	auto renderables = currentScene->getEntityRegistry().group<
		scene::components::MeshRendererComponent,
		scene::components::TransformComponent
	>
	(
		entt::get<scene::components::MetaComponent>
	);

	for (auto e : renderables) {
		auto [meshRenderer, transform, meta] = renderables.get(e);

		// Validate
		if (!validateEntity(meta, meshRenderer)) {
			continue;
		}

		// Check if draw call already exists
		size_t hash = utility::combineHash(meshRenderer.MeshHandle->getHandle(), meshRenderer.MaterialHandle->getHandle());

		if (m_lookup.find(hash) == m_lookup.end()) {
			// Create new drawcall and insert
			m_drawcalls.insert(m_drawcalls.end(), DrawCall{
				meshRenderer.MeshHandle->getVboHandle(),
				meshRenderer.MeshHandle->getIboHandle(),
				meshRenderer.MaterialHandle->getFields().Type->getShaderHandle(),
				meshRenderer.MaterialHandle->getFields().Type->getUniformHandle(),
				meshRenderer.MaterialHandle->getSamplerData(),
				meshRenderer.MaterialHandle->getPropertyData(),
				std::vector<scene::components::TransformComponent*>()
				});
			m_lookup[hash] = m_drawcalls.size() - 1;
		}

		m_drawcalls[m_lookup[hash]].Transformations.push_back(&transform);
	}

	state->setDrawCallList(&m_drawcalls);
}

bool EntityProvideOperation::validateEntity(scene::components::MetaComponent& meta, scene::components::MeshRendererComponent& mrenderer) {
	// TODO: Check if enabled
	asset::MeshAsset* mesh = mrenderer.MeshHandle;
	asset::MaterialAsset* material = mrenderer.MaterialHandle;

	if (mesh == nullptr ||
		material == nullptr) {
		return false;
	}

	asset::MaterialTypeAsset* materialType = material->getFields().Type;

	if (materialType == nullptr) {
		return false;
	}

	if (material->getPropertyData() == nullptr) {
		LOG_WARN("nullptr property data for {0}", meta.Name);
		return false;
	}

	// Check if they are valid
	bool isValid = true;
	if (!bgfx::isValid(mesh->getVboHandle()) ||
		!bgfx::isValid(mesh->getIboHandle())) {
		LOG_WARN("Invalid mesh passed for {0}", meta.Name);
		// Try to load it, and load it fast, cause we need it
		::aderite::Engine::getLoaderPool()->enqueue(mesh, io::LoaderPool::Priority::HIGH);
		isValid = false;
	}

	if (!bgfx::isValid(materialType->getShaderHandle()) ||
		!bgfx::isValid(materialType->getUniformHandle())) {
		LOG_WARN("Invalid material passed for {0}", meta.Name);
		// Try to load it, and load it fast, cause we need it
		::aderite::Engine::getLoaderPool()->enqueue(materialType, io::LoaderPool::Priority::HIGH);
		isValid = false;
	}

	for (asset::TextureAsset* ta : material->getFields().Samplers) {
		if (!bgfx::isValid(ta->getTextureHandle())) {
			LOG_WARN("Invalid texture passed for {0}", meta.Name);
			// Try to load it, and load it fast, cause we need it
			::aderite::Engine::getLoaderPool()->enqueue(ta, io::LoaderPool::Priority::HIGH);
			isValid = false;
		}
	}

	return isValid;
}

void EntityProvideOperation::cleanList() {
	m_drawcalls.clear();
	m_lookup.clear();
}

reflection::Type EntityProvideOperation::getType() const {
	return static_cast<reflection::Type>(reflection::RuntimeTypes::OP_ENTITY);
}

bool EntityProvideOperation::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	return true;
}

bool EntityProvideOperation::deserialize(io::Serializer* serializer, const YAML::Node& data) {
	return true;
}

ADERITE_RENDERING_NAMESPACE_END
