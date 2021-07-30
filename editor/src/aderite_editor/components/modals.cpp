#include "modals.hpp"

#include <imgui/imgui.h>

#include "aderite/aderite.hpp"
#include "aderite/utility/log.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			namespace modals {
				constexpr size_t input_length = 64;

				static int InputTextCallback(ImGuiInputTextCallbackData* data) {
					if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
						// Resize string callback
						// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
						std::string* str = (std::string*)data->UserData;
						//IM_ASSERT(data->Buf == str->c_str());
						str->resize(data->BufTextLen);
						data->Buf = (char*)str->c_str();
					}
					return 0;
				}

				text_input::text_input(const std::string& title, const std::string& text, const Options& options)
					: m_text(text), m_title(title), m_options(options)
				{
					m_text.resize(input_length, ' ');
				}

				void text_input::show() {
					m_show = true;
				}

				void text_input::set_text(const std::string& text) {
					m_text = text;
				}

				void text_input::set_title(const std::string& text) {
					m_title = text;
				}

				void text_input::set_confirm_action(ConfirmFn fn) {
					m_confirm = std::move(fn);
				}

				void text_input::set_cancel_action(CancelFn fn) {
					m_cancel = std::move(fn);
				}

				void text_input::set_confirm_button_text(const std::string& text) {
					m_confirm_text = text;
				}

				void aderite::editor::components::modals::text_input::render() {
					if (m_show) {
						ImGui::OpenPopup(m_title.c_str());

						// Reset 
						m_value = "";
						m_show = false;
					}

					// Center on screen
					ImVec2 center = ImGui::GetMainViewport()->GetCenter();
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

					// Input flags
					ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_CallbackResize;
					if (!m_options.AllowEmpty) {
						input_flags |= ImGuiInputTextFlags_CharsNoBlank;
					}

					if (ImGui::BeginPopupModal(m_title.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("%s", m_text.c_str());
						ImGui::SameLine();
						ImGui::InputText("", (char*)m_value.c_str(), m_value.capacity() + 1, input_flags, InputTextCallback, &m_value);
						ImGui::Separator();

						float width = ImGui::GetContentRegionAvail().x * 0.4855f;
						if (ImGui::Button(m_confirm_text.c_str(), ImVec2(width, 0.0f))) {
							if (m_confirm) {
								bool valid = true;

								if (!m_options.AllowEmpty) {
									if (m_value.empty() || m_value.find_first_not_of(' ') == std::string::npos) {
										valid = false;
									}
								}

								if (valid) {
									m_confirm(m_value);
								}
								else {
									// Cancel if not valid
									// TODO: Error text?
									if (m_cancel) {
										m_cancel();
									}
								}
							}

							ImGui::CloseCurrentPopup(); 
						}

						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						
						if (ImGui::Button("Cancel", ImVec2(width, 0.0f))) {
							if (m_cancel) {
								m_cancel();
							}

							ImGui::CloseCurrentPopup(); 
						}

						ImGui::EndPopup();
					}
				}

			}
		}
	}
}
