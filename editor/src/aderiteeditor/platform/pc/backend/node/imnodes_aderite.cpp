#include "imnodes_aderite.hpp"

#include "aderite/utility/Macros.hpp"

#include "imnodes_internal.h"

namespace IMNODES_NAMESPACE {

void SetLinkUserData(int id, void* data) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Links, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    editor.Links.Pool[idx].UserData = data;
}

void* GetLinkUserData(int id) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Links, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    return editor.Links.Pool[idx].UserData;
}

void SetNodeUserData(int id, void* data) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Nodes, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    editor.Nodes.Pool[idx].UserData = data;
}

void* GetNodeUserData(int id) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Nodes, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    return editor.Nodes.Pool[idx].UserData;
}

void SetPinUserData(int id, void* data) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Pins, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    editor.Pins.Pool[idx].UserData = data;
}

void* GetPinUserData(int id) {
    ImNodesEditorContext& editor = EditorContextGet();
    int idx = ObjectPoolFind(editor.Pins, id);
    ADERITE_DYNAMIC_ASSERT(idx != -1, "Invalid index");
    return editor.Pins.Pool[idx].UserData;
}

ImVec2 ToEditorGridPos(const ImVec2& pos) {
    ImNodesEditorContext& editor = EditorContextGet();
    return pos - GImNodes->CanvasOriginScreenSpace - editor.Panning;
}

} // namespace IMNODES_NAMESPACE
