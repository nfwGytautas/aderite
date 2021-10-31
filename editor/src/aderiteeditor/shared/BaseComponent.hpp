#pragma once

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Base component of all editor components
 */
class BaseComponent {
public:
    virtual ~BaseComponent() {}

    /**
     * @brief Initialize the component, this is called after the engine has initialized
     * it's rendering engine so the component can use graphics API if needed
     */
    virtual void init() {}

    /**
     * @brief Shutdown the component, this is called when the editor is about to close,
     * so it the component allocated any graphics objects on init then it should destroy
     * them in this method
     */
    virtual void shutdown() {}

    /**
     * @brief Render the component
     */
    virtual void render() = 0;
};

} // namespace editor
} // namespace aderite
