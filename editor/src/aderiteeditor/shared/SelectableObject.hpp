#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/scene/Forward.hpp"

#include "aderiteeditor/extensions/Forward.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Types for selectable types
 */
enum class SelectableObjectType {
    None,
    Asset,
    Serializable,
    Entity,
};

/**
 * @brief Selectable object, that can be any of SelectableObjectType
 */
class SelectableObject {
public:
    SelectableObject();
    SelectableObject(io::SerializableObject* asset);
    SelectableObject(scene::EditorEntity* entity);
    SelectableObject(io::ISerializable* serializable);

    /**
     * @brief Returns the type of the object
     */
    SelectableObjectType getType() const;

    /**
     * @brief Returns the object part of the selectable
     */
    io::SerializableObject* getAsset() const;

    /**
     * @brief Returns the entity of the selectable
     */
    scene::EditorEntity* getEntity() const;

    /**
     * @brief Returns the object part of the selectable
     */
    io::ISerializable* getSerializable() const;

private:
    SelectableObjectType m_type = SelectableObjectType::None;

    union {
        io::SerializableObject* Object;
        io::ISerializable* Serializable;
        scene::EditorEntity* Entity;
    } m_data;
};

} // namespace editor
} // namespace aderite
