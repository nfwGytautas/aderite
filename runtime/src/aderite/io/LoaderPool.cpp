#include "LoaderPool.hpp"

#include "aderite/io/ILoadable.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/LogExtensions.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

LoaderPool::LoaderPool() : LoaderPool(std::thread::hardware_concurrency()) {}

LoaderPool::LoaderPool(size_t threadCount) {
    ADERITE_LOG_BLOCK;
    LOG_DEBUG("[IO] Loader pool is created with {0} threads", threadCount);
    for (size_t i = 0; i < threadCount; i++) {
        m_loaders.push_back(new Loader(this));
        m_loaders[i]->startup();
    }

    // Wait until all loaders are ready
    LOG_TRACE("[IO] Waiting for all loaders to spin up");
    for (Loader* loader : m_loaders) {
        // Shouldn't take long so no need to complicate the logic
        while (!loader->isReady()) {
        }
    }

    LOG_INFO("[IO] Loader pool created and initialized");
}

LoaderPool::~LoaderPool() {
    ADERITE_LOG_BLOCK;
    LOG_TRACE("[IO] Loader pool is shutting down");
    m_terminated = true;

    for (size_t i = 0; i < m_loaders.size(); i++) {
        LOG_TRACE("[IO] Terminating loader {0}", i);
        m_loaders[i]->terminate();
    }

    m_cvAdded.notify_all();

    for (size_t i = 0; i < m_loaders.size(); i++) {
        delete m_loaders[i];
    }

    m_loaders.clear();

    LOG_INFO("[IO] Loader pool shutdown");
}

void LoaderPool::enqueue(ILoadable* loadable, Priority priority) {
    std::unique_lock<std::mutex> lock(m_lock);

    if (this->isLoading(loadable)) {
        // Already being loaded
        return;
    }

    if (!loadable->needsLoading()) {
        // Doesn't need to be loaded
        return;
    }

    LOG_TRACE("[IO] Loadable enqueued");

    switch (priority) {
    case Priority::LOW: {
        // At the back of the queue
        m_queue.insert(m_queue.end(), loadable);
        break;
    }
    case Priority::NORMAL: {
        // Middle
        m_queue.insert(m_queue.begin() + m_highEnd, loadable);
        break;
    }
    case Priority::HIGH: {
        // Front
        m_highEnd++;
        m_queue.insert(m_queue.begin(), loadable);
    }
    }

    m_cvAdded.notify_one();
}

ILoadable* LoaderPool::getNextLoadable() {
    std::unique_lock<std::mutex> latch(m_lock);
    m_cvAdded.wait(latch, [this]() {
        return m_terminated || !m_queue.empty();
    });

    if (m_terminated) {
        return nullptr;
    }

    ILoadable* loadable = m_queue.front();
    m_queue.erase(m_queue.begin());
    if (m_highEnd > 0) {
        m_highEnd--;
    }

    LOG_TRACE("[IO] Loadable popped");
    ADERITE_DYNAMIC_ASSERT(loadable != nullptr, "Nullptr loadable being passed from a pool that is not terminated");
    return loadable;
}

bool LoaderPool::isLoading(ILoadable* loadable) const {
    auto it = std::find(m_queue.begin(), m_queue.end(), loadable);

    if (it == m_queue.end()) {
        // Check if loader is currently loading it
        for (Loader* l : m_loaders) {
            if (l->current() == loadable) {
                return true;
            }
        }

        return false;
    }

    return true;
}

} // namespace io
} // namespace aderite
