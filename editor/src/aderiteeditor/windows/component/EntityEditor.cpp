#include "EntityEditor.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/MeshAsset.hpp" 
#include "aderite/asset/MaterialAsset.hpp" 
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/shared/IEventSink.hpp"
#include "aderiteeditor/windows/component/ComponentUtility.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

template<typename Component, typename RenderFn>
bool render_component(const std::string& label, ::aderite::scene::Entity& Entity, RenderFn renderFn) {
	const ImGuiTreeNodeFlags treeNodeFlags = /*ImGuiTreeNodeFlags_DefaultOpen | */ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	if (Entity.hasComponent<Component>()) {
		auto& component = Entity.getComponent<Component>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		
		//ImGui::Separator();

		bool open = ImGui::TreeNodeEx((void*)typeid(Component).hash_code(), treeNodeFlags, label.c_str());
		ImGui::PopStyleVar();

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open) {
			renderFn(component);
			ImGui::TreePop();
		}

		if (removeComponent) {
			Entity.removeComponent<Component>();
		}

		return true;
	}

	return false;
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
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
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
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

	bool hasTransform = render_component<::aderite::scene::components::TransformComponent>("Transform", m_selectedEntity, 
		[](::aderite::scene::components::TransformComponent& c) {
		DrawVec3Control("Position", c.Position);
		glm::vec3 rotation = glm::degrees(c.Rotation);
		DrawVec3Control("Rotation", rotation);
		c.Rotation = glm::radians(rotation);
		DrawVec3Control("Scale", c.Scale, 1.0f);
	});

	bool hasMeshRenderer = render_component<::aderite::scene::components::MeshRendererComponent>("Mesh Renderer", m_selectedEntity,
		[](::aderite::scene::components::MeshRendererComponent& c) {

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
	});

	bool hasAll = hasTransform && hasMeshRenderer;

	if (!hasAll) {
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

			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void EntityEditor::setActiveEntity(scene::Entity& Entity) {
	m_selectedEntity = Entity;
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
