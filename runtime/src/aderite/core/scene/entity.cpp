#include "entity.hpp"

namespace aderite {
	namespace scene {

		entity::entity(entt::entity handle, scene* scene)
			: m_handle(handle), m_scene(scene)
		{}

	}
}
