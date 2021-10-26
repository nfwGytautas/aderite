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
#include "aderite/scripting/FieldWrapper.hpp"

namespace aderite {
namespace scripting {

BehaviorWrapper::BehaviorWrapper(MonoImage* image, MonoClass* klass)
	: m_image(image), m_class(klass)
{
	// Just for debugging purposes
	printClassMethods(klass);

	// Gather meta information
	const std::string nSpace = std::string(mono_class_get_namespace(klass));
	const std::string name = std::string(mono_class_get_name(klass));
	m_name = nSpace + (!nSpace.empty() ? "." : "") + name;

	m_updateMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"Update",
		{{ "single" }},
		false
	}.value());

	// Resolve fields
	this->resolveFields();

	// Create thunks for frequent functions
	if (this->hasUpdateFunction()) {
		m_updateThunk = static_cast<UpdateThunkFn*>(mono_method_get_unmanaged_thunk(m_updateMethod));
	}
}

BehaviorWrapper::~BehaviorWrapper() {
	for (FieldWrapper* fw : m_fields) {
		delete fw;
	}
}

bool BehaviorWrapper::hasUpdateFunction() const {
	return m_updateMethod != nullptr;
}

void BehaviorWrapper::update(MonoObject* instance, float delta) const{
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_updateThunk(instance, delta, &ex);
}

MonoObject* BehaviorWrapper::createInstance() {
	return mono_object_new(::aderite::Engine::getScriptManager()->getDomain(), m_class);
}

const std::vector<FieldWrapper*>& BehaviorWrapper::getFields() const {
	return m_fields;
}

FieldWrapper* BehaviorWrapper::getField(const std::string& fieldName) const {
	auto it = std::find_if(m_fields.begin(), m_fields.end(), [fieldName](FieldWrapper* fw) {
		return fw->getName() == fieldName;
	});

	if (it == m_fields.end()) {
		return nullptr;
	}

	return *it;
}

std::string BehaviorWrapper::getName() const {
	return m_name;
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

void BehaviorWrapper::resolveFields() {
	void* iter = NULL;
	MonoClassField* field;
	while (field = mono_class_get_fields(m_class, &iter)) {
		m_fields.push_back(new FieldWrapper(field));
	}
}

}
}
