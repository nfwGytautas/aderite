#include "LoaderPool.hpp"
#include <thread>

#include "aderite/io/Loader.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"

namespace aderite {
namespace io {

LoaderPool::LoaderPool() : LoaderPool(std::thread::hardware_concurrency()) {}

LoaderPool::LoaderPool(size_t threadCount) {
    for (size_t i = 0; i < threadCount; i++) {
        m_loaders.push_back(new Loader(this));
        m_loaders[i]->startup();
    }
}

LoaderPool::~LoaderPool() {
    m_terminated = true;

    for (size_t i = 0; i < m_loaders.size(); i++) {
        m_loaders[i]->terminate();
    }

    m_cvAdded.notify_all();

    for (size_t i = 0; i < m_loaders.size(); i++) {
        delete m_loaders[i];
    }

    m_loaders.clear();
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

    ADERITE_DYNAMIC_ASSERT(loadable != nullptr, "Nullptr loadable being passed from a pool that is not terminated");
    return loadable;
}

bool LoaderPool::isLoading(ILoadable* loadable) {
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
