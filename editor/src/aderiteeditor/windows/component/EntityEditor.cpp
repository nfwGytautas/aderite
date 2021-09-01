#include "EntityEditor.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/MaterialAsset.hpp" 
#include "aderite/physics/PhysicsController.hpp"
#include "aderite/physics/ColliderList.hpp"
#include "aderite/physics/Collider.hpp"
#include "aderite/physics/collider/BoxCollider.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/windows/component/ComponentUtility.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

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

static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	bool altered = false;

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize)) {
		values.x = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize)) {
		values.y = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize)) {
		values.z = resetValue;
		altered = true;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	if (ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f")) {
		altered = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	return altered;
}

EntityEditor::EntityEditor()
{}

EntityEditor::~EntityEditor()
{}

void EntityEditor::render() {
	static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
	static bool renaming = false;
	static bool appearing = false;
	static std::string rename_value = "";

	if (!ImGui::Begin("Entity editor")) {
		ImGui::End();
		return;
	}

	if (m_selectedEntity == scene::Entity::null()) {
		ImGui::Text("Select Entity from scene view");
		ImGui::End();
		return;
	}

	// Component list
	if (!m_selectedEntity.hasComponent<::aderite::scene::components::MetaComponent>()) {
		// This shouldn't happen
		LOG_ERROR("Entity without MetaComponent information selected");
		shared::State::Sink->onSelectedEntityChanged(scene::Entity::null());
		ImGui::End();
		return;
	}

	// Meta
	auto& MetaComponent = m_selectedEntity.getComponent<::aderite::scene::components::MetaComponent>();

	if (!renaming) {
		if (ImGui::Selectable(MetaComponent.Name.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				rename_value = MetaComponent.Name;
				renaming = true;
				appearing = true;
			}
		}
	}
	else {
		if (DynamicInputText("##rename", &rename_value, edit_flags)) {
			renaming = false;

			if (!rename_value.empty()) {
				// TODO: Check for name conflict
				MetaComponent.Name = rename_value;
			}
		}

		if (!ImGui::IsItemActive() && !appearing) {
			renaming = false;
		}

		if (appearing) {
			ImGui::SetKeyboardFocusHere();
			appearing = false;
		}
	}

	// Other components as tree nodes that can be collapsed
	ImGui::Separator();

	bool hasTransform = m_selectedEntity.hasComponent<scene::components::TransformComponent>();
	if (hasTransform) {
		renderTransform();
	}

	bool hasMeshRenderer = m_selectedEntity.hasComponent<scene::components::MeshRendererComponent>();
	if (hasMeshRenderer) {
		renderMeshrenderer();
	}

	bool hasRigidbody = m_selectedEntity.hasComponent<scene::components::RigidbodyComponent>();
	if (hasRigidbody) {
		renderRigidbody();
	}

	if (m_selectedEntity.hasComponent<scene::components::CollidersComponent>()) {
		auto& colliders = m_selectedEntity.getComponent<scene::components::CollidersComponent>();
		std::vector<physics::Collider*> toRemove;

		// Colliders
		size_t i = 0;
		for (physics::Collider* c : *colliders.Colliders) {
			bool remove = false;

			switch (c->getType()) {
			case physics::ColliderType::BOX: {
				renderBoxCollider(i, c, remove);
				break;
			}
			default: {
				LOG_WARN("Unknown collider type");
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
	}

	ImGui::Separator();

	float width = ImGui::GetContentRegionAvail().x * 0.4855f;
	ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - width) * 0.5f);
	if (ImGui::Button("Add component", ImVec2(width, 0.0f))) {
		ImGui::OpenPopup("AddComponent");
	}

	if (ImGui::BeginPopup("AddComponent")) {
		if (!hasTransform && ImGui::MenuItem("Transform")) {
			m_selectedEntity.addComponent<::aderite::scene::components::TransformComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (!hasMeshRenderer && ImGui::MenuItem("Mesh Renderer")) {
			m_selectedEntity.addComponent<::aderite::scene::components::MeshRendererComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (!hasRigidbody && ImGui::MenuItem("Rigid body")) {
			m_selectedEntity.addComponent<::aderite::scene::components::RigidbodyComponent>();
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Box collider")) {
			if (!m_selectedEntity.hasComponent<::aderite::scene::components::CollidersComponent>()) {
				auto& collidersComponent = m_selectedEntity.addComponent<::aderite::scene::components::CollidersComponent>();
			}

			auto& collidersComponent = m_selectedEntity.getComponent<::aderite::scene::components::CollidersComponent>();
			collidersComponent.Colliders->addCollider(new physics::collider::BoxCollider());

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::End();
}

void EntityEditor::setActiveEntity(scene::Entity& Entity) {
	m_selectedEntity = Entity;
}

void EntityEditor::renderTransform() {
	bool open, remove = false;
	render_component_shared("Transform", "Transform", open, remove);

	if (open) {
		auto& c = m_selectedEntity.getComponent<scene::components::TransformComponent>();

		if (DrawVec3Control("Position", c.Position)) {
			c.WasAltered = true;
		}

		glm::vec3 euler = glm::eulerAngles(c.Rotation);
		glm::vec3 rotation = glm::degrees(euler);
		if (DrawVec3Control("Rotation", rotation)) {
			c.Rotation = glm::quat(rotation);
			c.WasAltered = true;
		}

		c.Rotation = glm::radians(rotation);
		if (DrawVec3Control("Scale", c.Scale, 1.0f)) {
			c.WasAltered = true;
		}

		ImGui::TreePop();
	}

	if (remove) {
		m_selectedEntity.removeComponent<scene::components::TransformComponent>();
	}
}

void EntityEditor::renderMeshrenderer() {
	bool open, remove = false;
	render_component_shared("Mesh renderer", "Mesh renderer", open, remove);

	if (open) {
		auto& c = m_selectedEntity.getComponent<scene::components::MeshRendererComponent>();

		if (ImGui::BeginTable("MeshRendererTable", 2)) {
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Mesh");

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-FLT_MIN);

			if (c.MeshHandle) {
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
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Material");

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-FLT_MIN);

			if (c.MaterialHandle) {
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
			}

			ImGui::EndTable();
		}

		ImGui::TreePop();
	}

	if (remove) {
		m_selectedEntity.removeComponent<scene::components::MeshRendererComponent>();
	}
}

void EntityEditor::renderRigidbody() {
	bool open, remove = false;
	render_component_shared("Rigidbody", "Rigidbody", open, remove);

	if (open) {
		auto& c = m_selectedEntity.getComponent<scene::components::RigidbodyComponent>();

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
		m_selectedEntity.removeComponent<scene::components::RigidbodyComponent>();
	}
}

void EntityEditor::renderBoxCollider(size_t idx, physics::Collider* collider, bool& remove) {
	bool open = false;
	render_component_shared("Box collider " + std::to_string(idx), "Box collider", open, remove);
	
	if (open) {
		auto typeCollider = static_cast<physics::collider::BoxCollider*>(collider);
		bool isTrigger = typeCollider->isTrigger();
		glm::vec3 size = typeCollider->getSize();

		if (ImGui::Checkbox("Is trigger", &isTrigger)) {
			typeCollider->setTrigger(isTrigger);
		}

		if (DrawVec3Control("Size", size, 1.0f)) {
			typeCollider->setSize(size);
		}

		ImGui::TreePop();
	}
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
