#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/scene/Entity.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

namespace aderite {
namespace editor_ui {

/**
 * @brief Object inspector used to edit entity and object properties
*/
class Inspector final : public shared::BaseComponent {
public:
	// Inherited via BaseComponent
	virtual void render() override;

private:
	void renderEntity();
	void renderTransform(scene::Entity entity);
	void renderMeshrenderer(scene::Entity entity);
	void renderRigidbody(scene::Entity entity);
	void renderAudioListener(scene::Entity entity);
	void renderAudioSource(scene::Entity entity);
	void renderColliders(scene::Entity entity);

	void renderAsset();
	void renderMesh(io::SerializableObject* asset);
	void renderTexture(io::SerializableObject* asset);
	void renderMaterial(io::SerializableObject* asset);
	void renderMaterialType(io::SerializableObject* asset);
	void renderScene(io::SerializableObject* asset);
};

}
}