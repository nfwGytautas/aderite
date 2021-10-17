#pragma once

#include <unordered_map>
#include "aderite/io/PathResolver.hpp"

namespace aderite {
namespace io {

/**
 * @brief A resolver that maps a name and handle to path by using a global asset map
*/
class MappedResolver : public PathResolver {
public:
	// Inherited via PathResolver
	virtual Path resolve(SerializableHandle handle);
	virtual void store(SerializableObject* object);
private:
	std::vector<Path> m_paths;
};

}
}
