#include "BehaviorWrapper.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

#include "aderite/Aderite.hpp"
#include "aderite/scripting/MonoUtils.hpp"
#include "aderite/scripting/ScriptManager.hpp"

namespace aderite {
namespace scripting {

BehaviorWrapper::BehaviorWrapper(MonoImage* image, MonoClass* klass)
	: m_image(image), m_class(klass)
{
	// Just for debugging purposes
	printClassMethods(klass);

	// Gather meta information
	m_updateMethod = resolveMethod(MethodSignature{
		mono_class_get_namespace(klass),
		mono_class_get_name(klass),
		"Update",
		{{ "single" }},
		false
	}.value());

	// Create thunks for frequent functions
	if (hasUpdateFunction()) {
		m_updateThunk = static_cast<UpdateThunkFn*>(mono_method_get_unmanaged_thunk(m_updateMethod));
	}
}

bool BehaviorWrapper::hasUpdateFunction() const {
	return m_updateMethod != nullptr;
}

void BehaviorWrapper::invokeUpdate(MonoObject* instance, float delta) const {
	MonoException* ex = nullptr;
	m_updateThunk(instance, delta, &ex);
}

MonoObject* BehaviorWrapper::createInstance() {
	return mono_object_new(::aderite::Engine::getScriptManager()->getDomain(), m_class);
}

MonoMethod* BehaviorWrapper::resolveMethod(const std::string& signature) {
	//Build a method description object
	MonoMethodDesc* methodDesc = mono_method_desc_new(signature.c_str(), NULL);

	//Search the method in the image
	MonoMethod* method = mono_method_desc_search_in_image(methodDesc, m_image);

	// Free memory
	mono_method_desc_free(methodDesc);

	return method;
}

}
}
