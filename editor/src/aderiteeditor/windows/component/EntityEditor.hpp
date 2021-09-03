#pragma once

#include "aderite/audio/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderite/physics/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Entity editor is a component that allows to edit specific selected Entity information
*/
class EntityEditor : public shared::BaseComponent {
public:
	EntityEditor();
	virtual ~EntityEditor();

	// Inherited via BaseComponent
	virtual void render() override;

	/**
	 * @brief Sets the current active Entity
	*/
	void setActiveEntity(scene::Entity& Entity);

private:
	void renderTransform();
	void renderMeshrenderer();
	void renderRigidbody();
	void renderAudioListener();

	void renderAudioSource(size_t idx, audio::AudioInstance* instance, bool& remove);
	void renderBoxCollider(size_t idx, physics::Collider* collider, bool& remove);
private:
	scene::Entity m_selectedEntity = scene::Entity::null();
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
