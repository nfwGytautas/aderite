#include "DragDrop.hpp"

#include <imgui/imgui.h>

namespace aderite {
namespace editor {

struct DragDropInfo {
    io::NamedSerializable* Data = nullptr;
};


void DragDrop::renderSource(io::NamedSerializable* serializable) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        DragDropInfo ddi {serializable};
        ImGui::SetDragDropPayload(std::to_string(serializable->getType()).c_str(), &ddi, sizeof(DragDropInfo));
        ImGui::EndDragDropSource();
    }
}

void DragDrop::renderSource(io::NamedSerializable* serializable, reflection::Type as) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload(std::to_string(as).c_str(), serializable, sizeof(io::NamedSerializable*));
        ImGui::EndDragDropSource();
    }
}

io::NamedSerializable* DragDrop::renderTarget(reflection::Type type) {
    io::NamedSerializable* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::to_string(type).c_str())) {
            result = static_cast<DragDropInfo*>(payload->Data)->Data;
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderGenericSource(io::NamedSerializable* serializable) {
    if (serializable == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("GenericDD", serializable, sizeof(io::NamedSerializable*));
        ImGui::EndDragDropSource();
    }
}

io::NamedSerializable* DragDrop::renderGenericTarget() {
    io::NamedSerializable* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GenericDD")) {
            result = static_cast<io::NamedSerializable*>(payload->Data);
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderDirectorySource(vfs::Directory* dir) {
    if (dir == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("DirectoryDD", dir, sizeof(vfs::Directory*));
        ImGui::EndDragDropSource();
    }
}

vfs::Directory* DragDrop::renderDirectoryTarget() {
    vfs::Directory* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DirectoryDD")) {
            result = static_cast<vfs::Directory*>(payload->Data);
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

void DragDrop::renderFileSource(vfs::File* file) {
    if (file == nullptr) {
        return;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("FileDD", file, sizeof(vfs::File*));
        ImGui::EndDragDropSource();
    }
}

vfs::File* DragDrop::renderFileTarget() {
    vfs::File* result = nullptr;

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FileDD")) {
            result = static_cast<vfs::File*>(payload->Data);
        }

        ImGui::EndDragDropTarget();
    }

    return result;
}

} // namespace editor
} // namespace aderite
