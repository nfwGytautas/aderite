#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
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

	friend class Loader;
private:
	std::vector<Loader*> m_loaders;
	std::mutex m_lock;
	std::condition_variable m_cvAdded;
	bool m_terminated = false;

	std::queue<ILoadable*> m_lowQueue;
	std::queue<ILoadable*> m_normalQueue;
	std::queue<ILoadable*> m_highQueue;
};

}
}
