#pragma once

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Class to specify that the class uses loader
 */
class ILoadable {
public:
    virtual ~ILoadable() {};

    /**
     * @brief Load the object data, this function is executed in a separate thread
     * @param loader Loader instance
     */
    virtual void load(const io::Loader* loader) = 0;

    /**
     * @brief Unload the object
     */
    virtual void unload() = 0;

    /**
     * @brief Returns true if the object needs loading, false otherwise
     */
    virtual bool needsLoading() const = 0;

private:
    friend class Loader;
    friend class LoaderPool;
};

} // namespace io
} // namespace aderite
