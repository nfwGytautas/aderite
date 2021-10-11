#include "AssetEditor.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Random.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderite/asset/TextureAsset.hpp"
#include "aderite/asset/property/Property.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/utility/Utility.hpp"
#include "aderiteeditor/compiler/Compiler.hpp"
#include "aderiteeditor/windows/component/ComponentUtility.hpp"
#include "aderiteeditor/windows/component/FileDialog.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

AssetEditor::AssetEditor()
{}

AssetEditor::~AssetEditor()
{}

void AssetEditor::render() {
	static ImGuiInputTextFlags edit_flags = ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue;
	static bool renaming = false;
	static bool appearing = false;
	static std::string rename_value = "";

	if (!ImGui::Begin("Asset editor")) {
		ImGui::End();
		return;
	}

	if (m_selectedAsset == nullptr) {
		ImGui::Text("Select asset from asset browser");
		ImGui::End();
		return;
	}

	if (!renaming) {
		if (ImGui::Selectable(m_selectedAsset->getName().c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				rename_value = std::filesystem::path(m_selectedAsset->getName()).stem().string();
				renaming = true;
				appearing = true;
			}
		}
	}
	else {
		if (DynamicInputText("##rename", &rename_value, edit_flags)) {
			renaming = false;

			if (!rename_value.empty()) {
				if (!::aderite::Engine::getAssetManager()->has(rename_value)) {
					rename_value = rename_value + std::filesystem::path(m_selectedAsset->getName()).extension().string();
					std::filesystem::rename(::aderite::Engine::getAssetManager()->getResDir() / m_selectedAsset->getName(), ::aderite::Engine::getAssetManager()->getResDir() / rename_value);
					m_selectedAsset->setName(rename_value);
				}
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

	// Type specific
	ImGui::Separator();

	// Type switch
	switch (m_selectedAsset->type()) {
	case asset::AssetType::SHADER: {
		shaderRender();
		break;
	}
	case asset::AssetType::MATERIAL: {
		materialRender();
		break;
	}
	case asset::AssetType::MESH: {
		meshRender();
		break;
	}
	case asset::AssetType::SCENE: {
		sceneRender();
		break;
	}
	case asset::AssetType::TEXTURE: {
		textureRender();
		break;
	}
	case asset::AssetType::MATERIAL_TYPE: {
		materialTypeRender();
		break;
	}
	default:
		ImGui::Text("Unsupported asset type");
	}

	ImGui::End();
}

void AssetEditor::setActiveAsset(asset::Asset* asset) {
	m_selectedAsset = asset;
}

void AssetEditor::shaderRender() {
	asset::ShaderAsset* shader = static_cast<asset::ShaderAsset*>(m_selectedAsset);
	asset::ShaderAsset::fields& finfo = shader->getFieldsMutable();

	if (ImGui::BeginTable("ShaderEditTable", 3)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed, 20.0f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Vertex program");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);

		if (!finfo.VertexPath.empty()) {
			ImGui::Button(finfo.VertexPath.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}
		else {
			ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__RawData)) {

			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TableSetColumnIndex(2);
		if (ImGui::Button("+###VertexSelect")) {
			std::string file = FileDialog::selectFile("Select vertex shader program source file");

			if (!file.empty()) {
				// Copy file to Raw
				std::filesystem::path filename = std::filesystem::path(file).filename();
				std::filesystem::path raw_dst = utility::makeUniquePath(::aderite::Engine::getAssetManager()->getRawDir() / filename);
				std::filesystem::copy_file(file, raw_dst);
				finfo.VertexPath = raw_dst.filename().string();
			}
		}

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Fragment program");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);

		if (!finfo.FragmentPath.empty()) {
			ImGui::Button(finfo.FragmentPath.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}
		else {
			ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__RawData)) {

			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TableSetColumnIndex(2);
		if (ImGui::Button("+###FragmentSelect")) {
			std::string file = FileDialog::selectFile("Select fragment shader program source file");

			if (!file.empty()) {
				std::filesystem::path filename = std::filesystem::path(file).filename();
				std::filesystem::path raw_dst = utility::makeUniquePath(::aderite::Engine::getAssetManager()->getRawDir() / filename);
				std::filesystem::copy_file(file, raw_dst);
				finfo.FragmentPath = raw_dst.filename().string();
			}
		}

		// TODO: Try compile button

		ImGui::EndTable();
	}
}

void AssetEditor::materialRender() {
	asset::MaterialAsset* material = static_cast<asset::MaterialAsset*>(m_selectedAsset);
	asset::MaterialAsset::fields& finfo = material->getFieldsMutable();

	if (ImGui::BeginCombo("Type", finfo.Type == nullptr ? "None" : finfo.Type->getName().c_str(), 0)) {
		for (asset::MaterialTypeAsset* mta : ::aderite::Engine::getAssetManager()->getAllOfType<asset::MaterialTypeAsset>(asset::AssetType::MATERIAL_TYPE)) {
			const bool is_selected = (finfo.Type == mta);
			if (ImGui::Selectable(mta->getName().c_str(), is_selected)) {
				material->setType(mta);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Properties:");

	if (ImGui::BeginTable("MaterialEditTable", 2)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

		if (finfo.Type != nullptr) {
			for (asset::prop::Property* p : finfo.Type->getFields().Properties) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(p->getName().c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(-FLT_MIN);

				switch (p->getType()) {
				case asset::prop::PropertyType::FLOAT: {
					float* val = p->getValue(material->getPropertyData());
					ImGui::InputFloat(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
					break;
				}
				case asset::prop::PropertyType::VEC2: {
					float* val = p->getValue(material->getPropertyData());
					ImGui::InputFloat2(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
					break;
				}
				case asset::prop::PropertyType::VEC3: {
					float* val = p->getValue(material->getPropertyData());
					ImGui::InputFloat3(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
					break;
				}
				case asset::prop::PropertyType::VEC4: {
					float* val = p->getValue(material->getPropertyData());
					ImGui::InputFloat4(std::string("#" + p->getName()).c_str(), p->getValue(material->getPropertyData()), NULL);
					break;
				}
				case asset::prop::PropertyType::TEXTURE_2D:
				case asset::prop::PropertyType::TEXTURE_CUBE: {
					// TODO: Verify type
					asset::TextureAsset* sampler = material->getFields().Samplers[p->getName()];

					if (sampler) {
						ImGui::Button(sampler->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}
					else {
						ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__TextureAsset)) {
							std::string name = static_cast<const char*>(payload->Data);
							asset::Asset* asset = ::aderite::Engine::getAssetManager()->getOrRead(name);
							if (asset) {
								sampler = static_cast<asset::TextureAsset*>(asset);

								if (!sampler->isLoaded()) {
									sampler->prepareLoad();
									sampler->load();
								}
							}
						}

						ImGui::EndDragDropTarget();
					}

					break;
				}
				default:
					ImGui::Text("Unknown property type");
				}
			}
		}

		ImGui::EndTable();
	}
}

void AssetEditor::meshRender() {
	asset::MeshAsset* mesh = static_cast<asset::MeshAsset*>(m_selectedAsset);
	asset::MeshAsset::fields& finfo = mesh->getFieldsMutable();

	if (ImGui::BeginTable("MeshEditTable", 3)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed, 20.0f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Source");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);

		if (!finfo.SourceFile.empty()) {
			ImGui::Button(finfo.SourceFile.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}
		else {
			ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__RawData)) {

			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TableSetColumnIndex(2);
		if (ImGui::Button("+###MeshSelect")) {
			std::string file = FileDialog::selectFile("Select mesh file");

			if (!file.empty()) {
				std::filesystem::path filename = std::filesystem::path(file).filename();
				std::filesystem::path raw_dst = utility::makeUniquePath(::aderite::Engine::getAssetManager()->getRawDir() / filename);
				std::filesystem::copy_file(file, raw_dst);
				finfo.SourceFile = raw_dst.filename().string();

				// Load it immediately
				mesh->prepareLoad();

				while (!mesh->isReadyToLoad()) {
					// TODO: Show loading screen
				}

				mesh->load();
			}
		}

		ImGui::EndTable();
	}
}

void AssetEditor::sceneRender() {
	scene::Scene* scene = static_cast<scene::Scene*>(m_selectedAsset);


}

void AssetEditor::textureRender() {
	asset::TextureAsset* texture = static_cast<asset::TextureAsset*>(m_selectedAsset);
	asset::TextureAsset::fields& finfo = texture->getFieldsMutable();

	// Needed for preview
	if (!texture->isLoaded() && !finfo.SourceFile.empty()) {
		texture->prepareLoad();
		texture->load();
	}

	if (ImGui::Checkbox("Is HDR", &finfo.IsHDR)) {
		// Unload previous
		texture->unload();
	}

	if (ImGui::BeginTable("TextureEditTable", 3)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);
		ImGui::TableSetupColumn("Add", ImGuiTableColumnFlags_WidthFixed, 20.0f);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Source");

		ImGui::TableSetColumnIndex(1);
		ImGui::PushItemWidth(-FLT_MIN);

		if (!finfo.SourceFile.empty()) {
			ImGui::Button(finfo.SourceFile.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}
		else {
			ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__RawData)) {

			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TableSetColumnIndex(2);
		if (ImGui::Button("+###TextureSelect")) {
			std::string file = FileDialog::selectFile("Select texture file");

			if (!file.empty()) {
				std::filesystem::path filename = std::filesystem::path(file).filename();
				std::filesystem::path raw_dst = utility::makeUniquePath(::aderite::Engine::getAssetManager()->getRawDir() / filename);
				std::filesystem::copy_file(file, raw_dst);
				finfo.SourceFile = raw_dst.filename().string();

				// Load it immediately
				texture->prepareLoad();

				while (!texture->isReadyToLoad()) {
					// TODO: Show loading screen
				}

				texture->load();
			}
		}

		ImGui::EndTable();
	}

	ImGui::Spacing();

	if (ImGui::BeginTable("TexturePreviewTable", 2)) {
		ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(1);
		ImGui::Image(
			(void*)(intptr_t)texture->getHandle().idx,
			ImVec2(96.0f, 96.0f),
			ImVec2(1, 0),
			ImVec2(0, 1),
			ImVec4(1, 1, 1, 1),
			ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("Preview");

		ImGui::EndTable();
	}
}

void AssetEditor::materialTypeRender() {
	static const std::string types[] = { "Texture 2D", "Texture Cube", "Float", "Vec2", "Vec3", "Vec4" };
	static const size_t typeCount = sizeof(types) / sizeof(*types);

	asset::MaterialTypeAsset* type = static_cast<asset::MaterialTypeAsset*>(m_selectedAsset);
	asset::MaterialTypeAsset::fields& finfo = type->getFieldsMutable();

	ImGui::Text("Properties:");

	if (ImGui::BeginTable("MaterialTypeEditTable", 3)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 130.0f);
		ImGui::TableSetupColumn("Spacing", ImGuiTableColumnFlags_WidthFixed, 10.0f);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_None);

		for (asset::prop::Property* prop : finfo.Properties) {
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

	}

	ImGui::Separator();
	if (ImGui::Button("Generate", ImVec2(ImGui::CalcItemWidth(), 0.0f))) {
		compiler::Compiler::compileMaterialType(type);
	}
	
	ImGui::PopItemWidth();

	if (ImGui::BeginPopup("SelectMaterialTypeProperty"))
	{
		ImGui::Text("Type");
		ImGui::Separator();
		for (int i = 0; i < typeCount; i++) {
			if (ImGui::Selectable(types[i].c_str())) {
				//selected_fish = i;
				finfo.Properties.push_back(new asset::prop::Property(static_cast<asset::prop::PropertyType>(i), "NewProperty_" + aderite::utility::generateString(8)));
				type->recalculate();
			}
		}
		ImGui::EndPopup();
	}

}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
