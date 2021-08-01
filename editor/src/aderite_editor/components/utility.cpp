#include "utility.hpp"

#include <imgui/imgui.h>

namespace aderite {
	namespace editor {
		namespace components {

			static int DynamicInputTextCallback(ImGuiInputTextCallbackData* data) {
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

			bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags) {
				return ImGui::InputText(label, (char*)value->c_str(), value->capacity() + 1, flags, DynamicInputTextCallback, value);
			}
		}
	}
}
