#include "AssetEditor.hpp"

#include <imgui/imgui.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/asset/Asset.hpp"
#include "aderite/asset/AssetManager.hpp"
#include "aderite/asset/ShaderAsset.hpp"
#include "aderite/asset/MaterialAsset.hpp"
#include "aderite/asset/MeshAsset.hpp"
#include "aderiteeditor/shared/State.hpp"
#include "aderiteeditor/shared/Config.hpp"
#include "aderiteeditor/utility/Utility.hpp"
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
							rename_value = m_selectedAsset->getName();
							renaming = true;
							appearing = true;
						}
					}
				}
				else {
					if (DynamicInputText("##rename", &rename_value, edit_flags)) {
						renaming = false;

						if (!rename_value.empty()) {
							if (::aderite::Engine::getAssetManager()->has(rename_value)) {
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
				case asset::AssetType::SHADER:
				{
					shaderRender();
					break;
				}
				case asset::AssetType::MATERIAL:
				{
					materialRender();
					break;
				}
				case asset::AssetType::MESH:
				{
					meshRender();
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

				if (ImGui::BeginTable("MaterialEditTable", 2)) {
					ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
					ImGui::TableSetupColumn("DD", ImGuiTableColumnFlags_None);

					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Shader");

					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(-FLT_MIN);

					if (finfo.Shader != nullptr) {
						ImGui::Button(finfo.Shader->getName().c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}
					else {
						ImGui::Button("None", ImVec2(ImGui::CalcItemWidth(), 0.0f));
					}

					if (ImGui::BeginDragDropTarget()) {
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(shared::DDPayloadID__ShaderAsset)) {
							std::string name = static_cast<const char*>(payload->Data);
							asset::Asset* asset = ::aderite::Engine::getAssetManager()->getByName(name);
							if (asset) {
								finfo.Shader = asset;
							}
						}

						ImGui::EndDragDropTarget();
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
						}
					}

					ImGui::EndTable();
				}
			}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
