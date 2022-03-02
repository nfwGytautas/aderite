#include "DragDrop.hpp"

#include <imgui/imgui.h>

namespace aderite {
namespace editor {

struct DragDropInfo {
    DragDropInfo(io::SerializableObject* object) {
        Object = object;
    }

    DragDropInfo(std::filesystem::path path) {
        File = path.string();
    }

    DragDropInfo(const std::filesystem::path& path, io::SerializableObject* object) : Object(object), File(path.string()) {}

    ~DragDropInfo() {}

    io::SerializableObject* Object = nullptr;
    std::string File = "";
};

void DragDrop::renderSource(io::SerializableObject* serializable) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        DragDropInfo ddi(serializable);
        ImGui::SetDragDropPayload(std::to_string(serializable->getType()).c_str(), &ddi, sizeof(DragDropInfo));
        ImGui::EndDragDropSource();
    }
}

void DragDrop::renderSource(io::SerializableObject* serializable, reflection::Type as) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload(std::to_string(as).c_str(), serializable, sizeof(io::SerializableObject*));
        ImGui::EndDragDropSource();
    }
}

io::SerializableObject* DragDrop::renderTarget(reflection::Type type) {
    io::SerializableObject* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::to_string(type).c_str())) {
            result = static_cast<DragDropInfo*>(payload->Data)->Object;
        }

        const ImGuiPayload* peekPayload = ImGui::GetDragDropPayload();
        if (peekPayload != nullptr) {
            if (peekPayload->IsDataType("AssetFileDD")) {
                if (static_cast<DragDropInfo*>(peekPayload->Data)->Object->getType() == type) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetFileDD")) {
                        result = static_cast<DragDropInfo*>(payload->Data)->Object;
                    }
                }
            }
        }


        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderGenericSource(io::SerializableObject* serializable) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("GenericDD", serializable, sizeof(io::SerializableObject*));
        ImGui::EndDragDropSource();
    }
}

io::SerializableObject* DragDrop::renderGenericTarget() {
    io::SerializableObject* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GenericDD")) {
            result = static_cast<io::SerializableObject*>(payload->Data);
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderFileSource(const std::filesystem::path& file) {
    if (ImGui::BeginDragDropSource()) {
        DragDropInfo ddi(file);
        ImGui::SetDragDropPayload("FileDD", &ddi, sizeof(DragDropInfo));
        ImGui::EndDragDropSource();
    }
}

std::filesystem::path DragDrop::renderFileTarget() {
    std::filesystem::path result;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileDD")) {
            result = static_cast<DragDropInfo*>(payload->Data)->File;
        }

        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AssetFileDD")) {
            result = static_cast<DragDropInfo*>(payload->Data)->File;
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderAssetFileSource(const std::filesystem::path& file, io::SerializableObject* object) {
    if (ImGui::BeginDragDropSource()) {
        DragDropInfo ddi(file, object);
        ImGui::SetDragDropPayload("AssetFileDD", &ddi, sizeof(DragDropInfo));
        ImGui::EndDragDropSource();
    }
}

} // namespace editor
} // namespace aderite
