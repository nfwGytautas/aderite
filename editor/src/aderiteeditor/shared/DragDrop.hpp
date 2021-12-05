#pragma once

#include <filesystem>

#include "aderite/io/SerializableObject.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Drag drop source wrapper for ImGui
 */
class DragDrop {
public:
    /**
     * @brief Renders ImGui necessary elements for creating a source
     */
    static void renderSource(io::SerializableObject* serializable);

    /**
     * @brief Renders ImGui necessary elements for creating a source with the specified type
     */
    static void renderSource(io::SerializableObject* serializable, reflection::Type as);

    /**
     * @brief Will render ImGui necessary elements for accepting a drag drop object of the specified type
     * @param type Type of the dragdrop object
     * @return io::Serializable instance in the drag drop payload
     */
    static io::SerializableObject* renderTarget(reflection::Type type);

    template<typename T>
    static T* renderTarget(reflection::RuntimeTypes type) {
        return static_cast<T*>(renderTarget(static_cast<reflection::Type>(type)));
    }

    /**
     * @brief Renders a generic drag drop source that can be accepted with renderGenericTarget
     */
    static void renderGenericSource(io::SerializableObject* serializable);

    /**
     * @brief Renders a generic drag drop target
     */
    static io::SerializableObject* renderGenericTarget();

    /**
     * @brief Renders a file drag drop source
     * @param file File to drag drop
     */
    static void renderFileSource(const std::filesystem::path& file);

    /**
     * @brief Renders a file drag drop target
     */
    static std::filesystem::path renderFileTarget();
};

} // namespace editor
} // namespace aderite