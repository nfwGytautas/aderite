#pragma once

#include "aderite/interfaces/iserializable.hpp"

namespace aderite {

	/**
	 * @brief Scene object, at any given time a single scene can be active in aderite.
	 * Scenes are guaranteed to be serializable. This includes all the possible resources that it uses
	 * be it meshes, materials, etc. However these resources are loaded as trunks into the asset manager
	 * the actual data is not loaded until needed.
	*/
	class scene : public interfaces::iserializable {
	public:

	private:

	};

}
