#include "ScriptEntity.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/utility/Log.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptList.hpp"
#include "aderite/scripting/Script.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scripting {

MonoObject* ResolveBehavior(scene::Scene* scene, entt::entity* entityHandle, MonoType* type) {
	scene::Entity entity(*entityHandle, scene);

	if (!entity.hasComponent<scene::components::ScriptsComponent>()) {
		return nullptr;
	}

	LOG_TRACE("Trying to resolve {0}", mono_type_get_name_full(type, MONO_TYPE_NAME_FORMAT_FULL_NAME));

	scene::components::ScriptsComponent& scripts = entity.getComponent<scene::components::ScriptsComponent>();
	for (scripting::Script* script : *scripts.Scripts) {
		
	}

	return nullptr;
}

void entityInternals() {
	mono_add_internal_call("Aderite.Log::__ResolveBehavior(intptr,uintptr,System.Type)", ResolveBehavior);
}

}
}
