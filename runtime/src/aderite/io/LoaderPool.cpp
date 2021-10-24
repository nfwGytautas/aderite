#include "LoaderPool.hpp"

#include <thread>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/Loader.hpp"

namespace aderite {
namespace io {

LoaderPool::LoaderPool() 
	: LoaderPool(std::thread::hardware_concurrency())
{}

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

	if (loadable->m_loading) {
		// Already in load queue
		return;
	}

	loadable->m_loading = true;

	switch (priority) {
	case Priority::LOW: {
		m_lowQueue.push(loadable);
		break;
	}
	case Priority::NORMAL: {
		m_normalQueue.push(loadable);
		break;
	}
	case Priority::HIGH: {
		m_highQueue.push(loadable);
	}
	}

	m_cvAdded.notify_one();
}

ILoadable* LoaderPool::getNextLoadable() {
	std::unique_lock<std::mutex> latch(m_lock);
	m_cvAdded.wait(latch, [this]() { return m_terminated || !m_lowQueue.empty() || !m_normalQueue.empty() || !m_highQueue.empty(); });

	if (m_terminated) {
		return nullptr;
	}

	ILoadable* loadable = nullptr;
	if (!m_highQueue.empty()) {
		loadable = m_highQueue.front();
		m_highQueue.pop();
	}
	else if (!m_normalQueue.empty()) {
		loadable = m_normalQueue.front();
		m_normalQueue.pop();
	}
	else if (!m_lowQueue.empty()) {
		loadable = m_lowQueue.front();
		m_lowQueue.pop();
	}
	ADERITE_DYNAMIC_ASSERT(loadable != nullptr, "Nullptr loadable being passed from a pool that is not terminated");
	return loadable;
}

void LoaderPool::onLoaded(ILoadable* loadable) {
	std::unique_lock<std::mutex> latch(m_lock);
	loadable->m_loading = false;
}

}
}
