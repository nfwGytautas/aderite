#pragma once

#include <vector>

#include "aderiteeditor/platform/pc/UIComponent.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Scene view is a component that displays all scene entities in a tree
 */
class SceneHierarchy : public UIComponent {
private:
    class ISceneHierarchyNode {
    public:
        virtual ~ISceneHierarchyNode() = default;

        virtual void render() = 0;
        virtual void contextMenu() = 0;
    };

    class ObjectNode : public ISceneHierarchyNode {
    public:
        void render() override;
        void contextMenu() override;
    };

    class AudioNode : public ISceneHierarchyNode {
    public:
        void render() override;
        void contextMenu() override;
    };

    class PhysicsRegionNode : public ISceneHierarchyNode {
    public:
        void render() override;
        void contextMenu() override;
    };

public:
    SceneHierarchy();
    virtual ~SceneHierarchy();

    // Inherited via UIComponent
    bool init() override;
    void shutdown() override;
    void render() override;

private:
    void contextMenu();

private:
    std::vector<ISceneHierarchyNode*> m_nodes;
};

} // namespace editor
} // namespace aderite
