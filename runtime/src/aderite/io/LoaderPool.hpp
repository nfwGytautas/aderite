#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>

#include "aderite/io/Forward.hpp"

namespace aderite {
namespace io {

/**
 * @brief Loader pool used to handle asset loading and managing loader instances
 */
class LoaderPool final {
public:
    /**
     * @brief Priority of loading
     */
    enum class Priority {
        LOW = 0,
        NORMAL = 1,
        HIGH = 2,
    };

public:
    /**
     * Creates a LoaderPool with max amount of threads that are allowed by the system
     */
    LoaderPool();

    /**
     * Creates a LoaderPool with specified thread count
     * @param threadCount The amount of threads inside the pool
     */
    LoaderPool(size_t threadCount);

    /**
     * Waits for all threads to finish and then cleans up
     */
    ~LoaderPool();

    /**
     * @brief Enqueues a loadable object to be loaded
     * @param loadable Loadable object
     * @param priority Priority with which to load
     */
    void enqueue(ILoadable* loadable, Priority priority = Priority::NORMAL);

private:
    /**
     * @brief Returns the next loadable instance that needs loading
     */
    ILoadable* getNextLoadable();

    /**
     * @brief Returns true if the specified loadable is already in load queue, false otherwise
     * @param loadable Loadable to check
     */
    bool isLoading(ILoadable* loadable) const;

    friend class Loader;

private:
    std::vector<Loader*> m_loaders;
    std::mutex m_lock;
    std::condition_variable m_cvAdded;
    bool m_terminated = false;

    size_t m_highEnd = 0;
    std::vector<ILoadable*> m_queue;
};

} // namespace io
} // namespace aderite
