#include "NodeEditor.hpp"

#include "aderiteeditor/windows/backend/node/imgui_node_editor.h"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN
namespace ed = ax::NodeEditor;
static ed::EditorContext* g_Context = nullptr;

NodeEditor::NodeEditor() {}

NodeEditor::~NodeEditor() {}

void NodeEditor::init() {
	g_Context = ed::CreateEditor();
}

void NodeEditor::render() {
    ed::SetCurrentEditor(g_Context);

    if (ImGui::Begin("Node editor")) {
        ed::Begin("My Editor");

        int uniqueId = 1;

        // Start drawing nodes.
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();

        ed::End();
    }

    ImGui::End();
}

void NodeEditor::shutdown() {
	ed::DestroyEditor(g_Context);
}

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
