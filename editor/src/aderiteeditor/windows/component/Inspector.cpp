#include "Inspector.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/components/Components.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/io/LoaderPool.hpp"
#include "aderite/io/Serializer.hpp"
#include "aderite/audio/AudioController.hpp"
#include "aderite/rendering/Pipeline.hpp"
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"

#include "aderiteeditor/utility/Utility.hpp"
#include "aderiteeditor/utility/ImGui.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/SelectableObject.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/shared/DragDropObject.hpp"
#include "aderiteeditor/asset/RenderingPipeline.hpp"
#include "aderiteeditor/asset/EditorMaterialType.hpp"
#include "aderiteeditor/asset/property/Property.hpp"
#include "aderiteeditor/vfs/File.hpp"
#include "aderiteeditor/vfs/VFS.hpp"
#include "aderiteeditor/compiler/PipelineEvaluator.hpp"
#include "aderiteeditor/compiler/ShaderEvaluator.hpp"
#include "aderiteeditor/node/Node.hpp"
#include "aderiteeditor/runtime/EditorTypes.hpp"
#include "aderiteeditor/windows/WindowsEditor.hpp"
#include "aderiteeditor/windows/component/FileDialog.hpp"
#include "aderiteeditor/windows/component/NodeEditor.hpp"

namespace aderite {
namespace editor_ui {

void render_component_shared(const std::string& id, const std::string& label, bool& open, bool& remove) {
	const ImGuiTreeNodeFlags treeNodeFlags = /*ImGuiTreeNodeFlags_DefaultOpen | */ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

	//ImGui::Separator();

	open = ImGui::TreeNodeEx(id.c_str(), treeNodeFlags, label.c_str());
	ImGui::PopStyleVar();

	ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
	if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
		ImGui::OpenPopup("ComponentSettings");
	}

	remove = false;
	if (ImGui::BeginPopup("ComponentSettings")) {
		if (ImGui::MenuItem("Remove component")) {
			remove = true;
		}

		ImGui::EndPopup();
	}
}

void Inspector::render() {
	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	switch (editor::State::LastSelectedObject.getType()) {
	case editor::SelectableObjectType::Asset: {
		this->renderAsset();
		break;
	}
	case editor::SelectableObjectType::Entity: {
		this->renderEntity();
		break;
	}
	default: {
		ImGui::Text("Select asset or entity");
	}
	}

	ImGui::End();
}

void Inspector::renderEntity() {
	static utility::InlineRename renamer;

	scene::Entity entity = editor::State::LastSelectedObject.getEntity();

	// Component list
	if (!entity.hasComponent<::aderite::scene::components::MetaComponent>()) {
		// This shouldn't happen
		ImGui::Text("Invalid entity");
		return;
	}

	// Meta
	auto& MetaComponent = entity.getComponent<aderite::scene::components::MetaComponent>();
	renamer.setValue(MetaComponent.Name);

	if (renamer.renderUI()) {
		MetaComponent.Name = renamer.getValue();
	}

	// Other components as tree nodes that can be collapsed
	ImGui::Separator();

	bool hasTransform = entity.hasComponent<scene::components::TransformComponent>();
	if (hasTransform) {
		this->renderTransform(entity);
	}

	bool hasMeshRenderer = entity.hasComponent<scene::components::MeshRendererComponent>();
	if (hasMeshRenderer) {
		this->renderMeshrenderer(entity);
	}

	bool hasRigidbody = entity.hasComponent<scene::components::RigidbodyComponent>();
	if (hasRigidbody) {
		this->renderRigidbody(entity);
	}

	bool hasAudioListener = entity.hasComponent<scene::components::AudioListenerComponent>();
	if (hasAudioListener) {
		this->renderAudioListener(entity);
	}

	bool hasAudioSource = entity.hasComponent<scene::components::AudioSourceComponent>();
	if (hasAudioSource) {
		this->renderAudioSource(entity);
	}

	bool hasColliders = entity.hasComponent<scene::components::CollidersComponent>();
	if (hasColliders) {
		this->renderColliders(entity);
	}

	// Add component
	ImGui::Separator();

	float width = ImGui::GetContentRegionAvail().x * 0.4855f;
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
	if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent")) {
		if (!hasTransform && ImGui::MenuItem("Transform")) {
			entity.addComponent<::aderite::scene::components::TransformComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (!hasMeshRenderer && ImGui::MenuItem("Mesh Renderer")) {
			entity.addComponent<::aderite::scene::components::MeshRendererComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (!hasRigidbody && ImGui::MenuItem("Rigidbody")) {
			entity.addComponent<::aderite::scene::components::RigidbodyComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (!hasAudioListener && ImGui::MenuItem("Audio Listener")) {
			entity.addComponent<::aderite::scene::components::AudioListenerComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Box collider")) {
			if (!entity.hasComponent<::aderite::scene::components::CollidersComponent>()) {
				auto& collidersComponent = entity.addComponent<::aderite::scene::components::CollidersComponent>();
			}

			auto& collidersComponent = entity.getComponent<::aderite::scene::components::CollidersComponent>();
			collidersComponent.Colliders->addCollider(new physics::BoxCollider());
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Audio source")) {
			auto& audioSourcesComponent = entity.addComponent<::aderite::scene::components::AudioSourceComponent>();
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Inspector::renderTransform(scene::Entity entity) {
	bool open, remove = false;
	render_component_shared("Transform", "Transform", open, remove);

	if (open) {
		auto& c = entity.getComponent<scene::components::TransformComponent>();

		if (utility::DrawVec3Control("Position", c.Position)) {
			c.WasAltered = true;
		}

		glm::vec3 euler = glm::eulerAngles(c.Rotation);
		glm::vec3 rotation = glm::degrees(euler);
		if (utility::DrawVec3Control("Rotation", rotation)) {
			c.Rotation = glm::quat(rotation);
			c.WasAltered = true;
		}

		c.Rotation = glm::radians(rotation);
		if (utility::DrawVec3Control("Scale", c.Scale, 1.0f)) {
			c.WasAltered = true;
		}

		ImGui::TreePop();
	}

	if (remove) {
		entity.removeComponent<scene::components::TransformComponent>();
	}
}

void Inspector::renderMeshrenderer(scene::Entity entity) {
	bool open, remove = false;
	render_component_shared("Mesh renderer", "Mesh renderer", open, remove);

	if (open) {
		auto& c = entity.getComponent<scene::components::MeshRendererComponent>();

		if (ImGui::BeginTable("MeshRendererTable", 2)) {
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Mesh");

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-FLT_MIN);

			/*if (c.MeshHandle) {
				ImGui::Button(c.MeshHandle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
			}
			else {
				ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__MeshAsset)) {
					std::string name = static_cast<const char*>(payload->Data);
					asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName(name);
					if (asset) {
						c.MeshHandle = static_cast<asset::MeshAsset*>(asset);
					}
				}

				ImGui::EndDragDropTarget();
			}*/

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Material");

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-FLT_MIN);

			/*if (c.MaterialHandle) {
				ImGui::Button(c.MaterialHandle->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
			}
			else {
				ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
			}

			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__MaterialAsset)) {
					std::string name = static_cast<const char*>(payload->Data);
					asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName(name);
					if (asset) {
						c.MaterialHandle = static_cast<asset::MaterialAsset*>(asset);
					}
				}

				ImGui::EndDragDropTarget();
			}*/

			ImGui::EndTable();
		}

		ImGui::TreePop();
	}

	if (remove) {
		entity.removeComponent<scene::components::MeshRendererComponent>();
	}
}

void Inspector::renderRigidbody(scene::Entity entity) {
	bool open, remove = false;
	render_component_shared("Rigidbody", "Rigidbody", open, remove);

	if (open) {
		auto& c = entity.getComponent<scene::components::RigidbodyComponent>();

		if (ImGui::Checkbox("Has gravity", &c.HasGravity)) {
			c.WasAltered = true;
		}
		if (ImGui::Checkbox("Is kinematic", &c.IsKinematic)) {
			c.WasAltered = true;
		}

		ImGui::Text("Mass");
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &c.Mass, 0.1f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
			c.WasAltered = true;
		}

		ImGui::TreePop();
	}

	if (remove) {
		entity.removeComponent<scene::components::RigidbodyComponent>();
	}
}

void Inspector::renderAudioListener(scene::Entity entity) {
	bool open, remove = false;
	render_component_shared("Audio Listener", "Audio Listener", open, remove);

	if (open) {
		auto& c = entity.getComponent<scene::components::AudioListenerComponent>();

		ImGui::Checkbox("Enabled", &c.IsEnabled);

		ImGui::TreePop();
	}

	if (remove) {
		entity.removeComponent<scene::components::AudioListenerComponent>();
	}
}

void Inspector::renderAudioSource(scene::Entity entity) {
	bool open, remove = false;
	render_component_shared("Audio source", "Audio Source", open, remove);

	if (open) {
		auto& c = entity.getComponent<scene::components::AudioSourceComponent>();

		//ImGui::Checkbox("Enabled", &c.IsEnabled);

		ImGui::TreePop();
	}

	if (remove) {
		entity.removeComponent<scene::components::AudioSourceComponent>();
	}
}

void Inspector::renderColliders(scene::Entity entity) {
	auto& colliders = entity.getComponent<scene::components::CollidersComponent>();
	std::vector<physics::Collider*> toRemove;

	// Colliders
	size_t i = 0;
	for (physics::Collider* c : *colliders.Colliders) {
		bool remove = false;

		switch (static_cast<reflection::RuntimeTypes>(c->getType())) {
		case reflection::RuntimeTypes::BOX_CLDR: {
			ImGui::Text("Box collider");
			break;
		}
		default: {
			continue;
		}
		}

		if (remove) {
			toRemove.push_back(c);
		}

		i++;
	}

	for (physics::Collider* c : toRemove) {
		colliders.Colliders->removeCollider(c);
	}

	//bool open = false;
	//render_component_shared("Box collider " + std::to_string(idx), "Box Collider", open, remove);

	//if (open) {
	//	auto typeCollider = static_cast<physics::collider::BoxCollider*>(collider);
	//	bool isTrigger = typeCollider->isTrigger();
	//	glm::vec3 size = typeCollider->getSize();

	//	if (ImGui::Checkbox("Is trigger", &isTrigger)) {
	//		typeCollider->setTrigger(isTrigger);
	//	}

	//	if (DrawVec3Control("Size", size, 1.0f)) {
	//		typeCollider->setSize(size);
	//	}

	//	ImGui::TreePop();
	//}
}

void Inspector::renderAsset() {
	static vfs::File* cacheFile = nullptr;
	static utility::InlineRename renamer;
	io::SerializableObject* object = editor::State::LastSelectedObject.getAsset();
	
	if (cacheFile == nullptr || cacheFile->getHandle() != object->getHandle()) {
		// Refind
		cacheFile = editor::State::Project->getVfs()->getFile(object->getHandle());
		ADERITE_DYNAMIC_ASSERT(cacheFile != nullptr, "Failed to find associated file");
		renamer.setValue(cacheFile->getName());
	}

	if (renamer.renderUI()) {
		editor::State::Project->getVfs()->rename(cacheFile, renamer.getValue());
		renamer.setValue(renamer.getValue());
	}

	ImGui::Separator();

	switch (static_cast<reflection::RuntimeTypes>(object->getType())) {
	case reflection::RuntimeTypes::MESH: {
		this->renderMesh(object);
		break;
	}
	case reflection::RuntimeTypes::TEXTURE: {
		this->renderTexture(object);
		break;
	}
	case reflection::RuntimeTypes::MATERIAL: {
		this->renderMaterial(object);
		break;
	}
	case reflection::RuntimeTypes::MAT_TYPE: {
		this->renderMaterialType(object);
		break;
	}
	case reflection::RuntimeTypes::SCENE: {
		this->renderScene(object);
		break;
	}
	case reflection::RuntimeTypes::PIPELINE:
	{
		this->renderPipeline(object);
		break;
	}
	default: {}
	}
}

void Inspector::renderMesh(io::SerializableObject* asset) {
	asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(asset);
	asset::MeshAsset::fields& finfo = mesh->getFieldsMutable();

	// Replace source button
	ImGui::PushItemWidth(-FLT_MIN);
	if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
		std::string file = component::FileDialog::selectFile("Select mesh file");

		if (!file.empty()) {
			::aderite::Engine::getFileHandler()->writePhysicalFile(mesh->getHandle(), file);
		}
	}
	ImGui::PopItemWidth();

	// Properties

	// Preview
	// TODO: Preview

}

void Inspector::renderTexture(io::SerializableObject* asset) {
	asset::TextureAsset* texture = static_cast<asset::TextureAsset*>(asset);
	asset::TextureAsset::fields& finfo = texture->getFieldsMutable();

	// Replace source button
	ImGui::PushItemWidth(-FLT_MIN);
	if (ImGui::Button("Replace source", ImVec2(-1.0f, 0.0f))) {
		std::string file = component::FileDialog::selectFile("Select image file");

		if (!file.empty()) {
			::aderite::Engine::getFileHandler()->writePhysicalFile(texture->getHandle(), file);
		}
	}
	ImGui::PopItemWidth();

	// Properties
	//if (ImGui::Checkbox("Is HDR", &finfo.IsHDR)) {
	//	// Unload previous
	//	texture->unload();
	//}

	// Needed for preview
	if (!bgfx::isValid(texture->getTextureHandle())) {
		ImGui::Text("Loading preview...");
		aderite::Engine::getLoaderPool()->enqueue(texture, io::LoaderPool::Priority::HIGH);
		// Can't show yet
		return;
	}

	// Preview
	// TODO: Can be remade to less bloated version
	if (ImGui::BeginTable("TexturePreviewTable", 2)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(1);
		ImGui::Image(
			(void*)(intptr_t)texture->getTextureHandle().idx,
			ImVec2(96.0f, 96.0f),
			ImVec2(1, 0),
			ImVec2(0, 1),
			ImVec4(1, 1, 1, 1),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("Preview");

		ImGui::EndTable();
	}
}

void Inspector::renderMaterial(io::SerializableObject* asset) {
	asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(asset);
	asset::MaterialAsset::fields& finfo = material->getFieldsMutable();

	//if (ImGui::BeginCombo("Type", finfo.Type == nullptr ? "None" : finfo.Type->getName().c_str(), 0)) {
	//	for (asset::MaterialTypeAsset* mta : ::aderite::Engine::getAssetManager()->getAllOfType<asset::MaterialTypeAsset>(asset::AssetType::MATERIAL_TYPE)) {
	//		const bool is_selected = (finfo.Type == mta);
	//		if (ImGui::Selectable(mta->getName().c_str(), is_selected)) {
	//			material->setType(mta);
	//		}

	//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
	//		if (is_selected) {
	//			ImGui::SetItemDefaultFocus();
	//		}
	//	}

	//	ImGui::EndCombo();
	//}

	ImGui::Separator();
	ImGui::Text("Properties:");

	asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(material->getFields().Type);

	if (type == nullptr) {
		ImGui::Text("Select material type...");
		return;
	}

	if (ImGui::BeginTable("MaterialEditTable", 2)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

		for (asset::Property* p : type->getProperties()) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text(p->getName().c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-FLT_MIN);

			switch (p->getType()) {
			case asset::PropertyType::FLOAT: {
				float* val = p->getValue(material->getPropertyData());
				ImGui::InputFloat(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
				break;
			}
			case asset::PropertyType::VEC2: {
				float* val = p->getValue(material->getPropertyData());
				ImGui::InputFloat2(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
				break;
			}
			case asset::PropertyType::VEC3: {
				float* val = p->getValue(material->getPropertyData());
				ImGui::InputFloat3(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
				break;
			}
			case asset::PropertyType::VEC4: {
				float* val = p->getValue(material->getPropertyData());
				ImGui::InputFloat4(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
				break;
			}
			case asset::PropertyType::TEXTURE_2D:
			case asset::PropertyType::TEXTURE_CUBE: {
				// TODO: Verify type
				//asset::TextureAsset*& sampler = finfo.Samplers[p->getName()];

				// TODO: Select sampler

				/*if (sampler) {
					ImGui::Button(sampler->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
				}
				else {
					ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
				}*/

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__TextureAsset)) {
						std::string name = static_cast<const char*>(payload->Data);
						/*asset::Asset* asset = ::aderite::Engine::getAssetManager()->getOrRead(name);
						if (asset) {
							sampler = static_cast<asset::TextureAsset*>(asset);

							if (!sampler->isLoaded()) {
								sampler->prepareLoad();
								sampler->load();
							}
						}*/
					}

					ImGui::EndDragDropTarget();
				}

				break;
			}
			default:
				ImGui::Text("Unknown property type");
			}
		}

		ImGui::EndTable();
	}
}

void Inspector::renderMaterialType(io::SerializableObject* asset) {
	static const std::string types[] = { "Texture 2D", "Texture Cube", "Float", "Vec2", "Vec3", "Vec4" };
	static const size_t typeCount = sizeof(types) / sizeof(*types);

	asset::EditorMaterialType* type = static_cast<asset::EditorMaterialType*>(asset);
	asset::MaterialTypeAsset::fields& finfo = type->getFieldsMutable();

	ImGui::Text("Properties:");

	if (ImGui::BeginTable("MaterialTypeEditTable", 3)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Spacing", ImGuiTableColumnFlags_WidthFixed, 10.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);

		for (asset::Property* prop : type->getProperties()) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text(prop->getName().c_str());

			ImGui::TableSetColumnIndex(2);
			ImGui::Text(types[static_cast<size_t>(prop->getType())].c_str());
		}

		ImGui::EndTable();
	}

	ImGui::PushItemWidth(-FLT_MIN);

	if (ImGui::Button("Add property", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		ImGui::OpenPopup("SelectMaterialTypeProperty");
	}

	ImGui::Separator();
	if (ImGui::Button("Open shader editor", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), component::NodeEditor::NodeEditorType::MATERIAL);
	}

	ImGui::Separator();
	if (ImGui::Button("Compile", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		type->compile();
	}

	ImGui::PopItemWidth();

	if (ImGui::BeginPopup("SelectMaterialTypeProperty")) {
		ImGui::Text("Type");
		ImGui::Separator();
		for (int i = 0; i < typeCount; i++) {
			if (ImGui::Selectable(types[i].c_str())) {
				//selected_fish = i;
				type->addProperty(new asset::Property(static_cast<asset::PropertyType>(i), "NewProperty_" + aderite::utility::generateString(8)));
				type->recalculate();
			}
		}
		ImGui::EndPopup();
	}
}

void Inspector::renderScene(io::SerializableObject* asset) {
	static vfs::File* cacheFile = nullptr;
	scene::Scene* type = static_cast<scene::Scene*>(asset);

	if (ImGui::BeginTable("SceneEditTable", 2)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Rendering pipeline:");
		
		ImGui::TableSetColumnIndex(1);
		if (type->getPipeline() == nullptr) {
			ImGui::Selectable("None");
		}
		else {
			vfs::File* file = editor::State::Project->getVfs()->getFile(type->getPipeline()->getHandle());
			ImGui::Selectable(file->getName().c_str());
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__PipelineAsset)) {
				editor::DragDropObject* ddo = static_cast<editor::DragDropObject*>(payload->Data);
				vfs::File* file = static_cast<vfs::File*>(ddo->Data);
				type->setPipeline(static_cast<rendering::Pipeline*>(::aderite::Engine::getSerializer()->getOrRead(file->getHandle())));
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::EndTable();
	}
}

void Inspector::renderPipeline(io::SerializableObject* asset) {
	asset::RenderingPipeline* type = static_cast<asset::RenderingPipeline*>(asset);

	ImGui::PushItemWidth(-FLT_MIN);
	if (ImGui::Button("Open editor", ImVec2(-1.0f, 0.0f))) {
		WindowsEditor::getInstance()->NodeEditor->setGraph(type->getGraph(), component::NodeEditor::NodeEditorType::RENDER_PIPELINE);
	}

	if (ImGui::Button("Compile", ImVec2(-1.0f, 0.0f))) {
		type->compile();
	}
	ImGui::PopItemWidth();
}

}
}
