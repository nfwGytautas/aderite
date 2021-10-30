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

	m_initMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"Init",
		{},
		false
		}.value());

	m_triggerEnterMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"OnTriggerEnter",
		{{ "Aderite.Entity" }},
		false
		}.value());

	m_triggerLeaveMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"OnTriggerLeave",
		{{ "Aderite.Entity" }},
		false
		}.value());

	m_collisionEnterMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"OnCollisionEnter",
		{{ "Aderite.Entity" }},
		false
		}.value());

	m_collisionLeaveMethod = this->resolveMethod(MethodSignature{
		nSpace,
		name,
		"OnCollisionLeave",
		{{ "Aderite.Entity" }},
		false
		}.value());

	// Resolve fields
	this->resolveFields();

	// Create thunks for frequent functions
	if (this->hasUpdateFunction()) {
		m_updateThunk = static_cast<UpdateThunkFn*>(mono_method_get_unmanaged_thunk(m_updateMethod));
	}

	if (this->hasTriggerEnterFunction()) {
		m_triggerEnterThunk = static_cast<TriggerThunkFn*>(mono_method_get_unmanaged_thunk(m_triggerEnterMethod));
	}

	if (this->hasTriggerLeaveFunction()) {
		m_triggerLeaveThunk = static_cast<TriggerThunkFn*>(mono_method_get_unmanaged_thunk(m_triggerLeaveMethod));
	}

	if (this->hasCollisionEnterFunction()) {
		m_collisionEnterThunk = static_cast<TriggerThunkFn*>(mono_method_get_unmanaged_thunk(m_collisionEnterMethod));
	}

	if (this->hasCollisionLeaveFunction()) {
		m_collisionLeaveThunk = static_cast<TriggerThunkFn*>(mono_method_get_unmanaged_thunk(m_collisionLeaveMethod));
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

bool BehaviorWrapper::hasInitFunction() const {
	return m_initMethod != nullptr;
}

bool BehaviorWrapper::hasTriggerEnterFunction() const {
	return m_triggerEnterMethod != nullptr;
}

bool BehaviorWrapper::hasTriggerLeaveFunction() const {
	return m_triggerLeaveMethod != nullptr;
}

bool BehaviorWrapper::hasCollisionEnterFunction() const {
	return m_collisionEnterMethod != nullptr;
}

bool BehaviorWrapper::hasCollisionLeaveFunction() const {
	return m_collisionLeaveMethod != nullptr;
}

void BehaviorWrapper::init(MonoObject* instance) const {
	// TODO: Handle exception
	MonoObject* ex = nullptr;
	mono_runtime_invoke(m_initMethod, instance, nullptr, &ex);
}

void BehaviorWrapper::update(MonoObject* instance, float delta) const{
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_updateThunk(instance, delta, &ex);
}

void BehaviorWrapper::onTriggerEnter(MonoObject* instance, MonoObject* trigger) const {
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_triggerEnterThunk(instance, trigger, &ex);
}

void BehaviorWrapper::onTriggerLeave(MonoObject* instance, MonoObject* trigger) const {
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_triggerLeaveThunk(instance, trigger, &ex);
}

void BehaviorWrapper::onCollisionEnter(MonoObject* instance, MonoObject* collision) const {
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_collisionEnterThunk(instance, collision, &ex);
}

void BehaviorWrapper::onCollisionLeave(MonoObject* instance, MonoObject* collision) const {
	// TODO: Handle exception
	MonoException* ex = nullptr;
	m_collisionLeaveThunk(instance, collision, &ex);
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

void BehaviorWrapper::pair(scene::Entity entity, MonoObject* instance) {
	MonoObject* obj = ::aderite::Engine::getScriptManager()->createEntityObject(entity);
	MonoClassField* entityField = ::aderite::Engine::getScriptManager()->getBehaviorEntityField();

	mono_field_set_value(instance, entityField, obj);

	// Initialize
	if (this->hasInitFunction()) {
		this->init(instance);
	}
}

std::string BehaviorWrapper::getName() const {
	return m_name;
}

MonoMethod* BehaviorWrapper::resolveMethod(const std::string& signature) {
	//Build a method description object
	MonoMethodDesc* methodDesc = mono_method_desc_new(signature.c_str(), 1);

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
		if (mono_field_get_flags(field) & MONO_FIELD_ATTR_PUBLIC) {
			m_fields.push_back(new FieldWrapper(field));
		}
	}
}

}
}
