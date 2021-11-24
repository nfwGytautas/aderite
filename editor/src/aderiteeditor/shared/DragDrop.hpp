#pragma once

#include "aderite/io/SerializableObject.hpp"

#include "aderiteeditor/vfs/Forward.hpp"

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
    static void renderSource(io::ISerializable* serializable);

    /**
     * @brief Renders ImGui necessary elements for creating a source with the specified type
     */
    static void renderSource(io::ISerializable* serializable, reflection::Type as);

    /**
     * @brief Will render ImGui necessary elements for accepting a drag drop object of the specified type
     * @param type Type of the dragdrop object
     * @return io::ISerializable instance in the drag drop payload
     */
    static io::ISerializable* renderTarget(reflection::Type type);

    template<typename T>
    static T* renderTarget(reflection::RuntimeTypes type) {
        return static_cast<T*>(renderTarget(static_cast<reflection::Type>(type)));
    }

    /**
     * @brief Renders a generic drag drop source that can be accepted with renderGenericTarget
     */
    static void renderGenericSource(io::ISerializable* serializable);

    /**
     * @brief Renders a generic drag drop target
     */
    static io::ISerializable* renderGenericTarget();

    /**
     * @brief Renders a directory drag drop source
    */
    static void renderDirectorySource(vfs::Directory* dir);

    /**
     * @brief Renders a directory drag drop target
    */
    static vfs::Directory* renderDirectoryTarget();

    /**
     * @brief Renders a file drag drop source
     */
    static void renderFileSource(vfs::File* file);

    /**
     * @brief Renders a file drag drop target
     */
    static vfs::File* renderFileTarget();
};

} // namespace editor
} // namespace aderite