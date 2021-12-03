#pragma once

#include "aderite/io/Forward.hpp"
#include "aderite/scene/Forward.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Types for selectable types
 */
enum class SelectableObjectType {
    None,
    Object,
};

/**
 * @brief Selectable object, that can be any of SelectableObjectType
 */
class SelectableObject {
public:
    SelectableObject();
    SelectableObject(io::SerializableObject* object);

    /**
     * @brief Returns the type of the object
     */
    SelectableObjectType getType() const;

    /**
     * @brief Returns the object part of the selectable
     */
    io::SerializableObject* getSerializable() const;

private:
    SelectableObjectType m_type = SelectableObjectType::None;

    union {
        io::SerializableObject* Object;
    } m_data;
};

} // namespace editor
} // namespace aderite
