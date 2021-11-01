#pragma once

#include <cstdint>
#include <memory>

#include "aderite/utility/Macros.hpp"

namespace aderite {

/**
 * @brief A special type of array that keeps a pool of objects, deleting these objects leave an empty address
 * @tparam T Object to be stored in the pool
 */
template<typename T>
class ObjectPool final {
public:
    /**
     * @brief Add object to the pool
     * @param object Object to add
     */
    void add(T* object) {
        this->findNextIndex();
        m_data[m_nextIndex] = object;
    }

    /**
     * @brief Remove object from the pool
     * @param object Object to remove
     * @returns Returns the object if found
     */
    T* remove(T* object) {
        ADERITE_DYNAMIC_ASSERT(object != nullptr, "Removing nullptr object from pool");

        // Find the index of the object
        for (size_t i = 0; i < m_size; i++) {
            if (m_data[i] == object) {
                T* object = m_data[i];
                m_data[i] = nullptr;
                this->findNextIndex(i);
                return object;
            }
        }

        return nullptr;
    }

private:
    /**
     * @brief Returns the next available index in the pool
     */
    void findNextIndex(size_t start = 0) const {
        while (start < m_size && m_data[start] != nullptr) {
            start++;
        }

        if (start >= m_size) {
            this->resize(m_size * 2);
            start++;
        }

        m_nextIndex = start;
    }

    /**
     * @brief Resizes the object pool
     * @param size New size of the pool
     */
    void resize(size_t size) {
        m_data = std::realloc(m_data, size);
        ADERITE_DYNAMIC_ASSERT(m_data != nullptr, "Failed to resize object pool");
    }

private:
    T* m_data = nullptr;
    size_t m_size = 0;
    size_t m_nextIndex = 0;
};

} // namespace aderite
