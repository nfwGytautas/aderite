#pragma once

#include <functional>
#include <string>

namespace aderite {
	namespace editor {
		namespace components {

			typedef int ImGuiInputTextFlags;

			/**
			 * @brief Expands upon ImGui InputText to provide resizing input field
			 * @param label Label of the field
			 * @param value Value to store in
			 * @param flags InputText flags
			 * @return Standard InputText return value
			*/
			bool DynamicInputText(const char* label, std::string* value, ImGuiInputTextFlags flags);
		}
	}
}