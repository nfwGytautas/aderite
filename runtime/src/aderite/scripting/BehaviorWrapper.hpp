#pragma once

#include <string>
#include <vector>
#include <mono/jit/jit.h>
#include "aderite/scripting/Forward.hpp"
#include "aderite/scene/Entity.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Special utility class for analyzing and using behavior marked classes
*/
class BehaviorWrapper final {
private:
	using UpdateThunkFn = void(MonoObject*, float, MonoException**);
	using TriggerThunkFn = void(MonoObject*, MonoObject*, MonoException**);
	using CollisionThunkFn = void(MonoObject*, MonoObject*, MonoException**);
public:
	BehaviorWrapper(MonoImage* image, MonoClass* klass);
	~BehaviorWrapper();

	/**
	 * @brief Returns true if the behavior has an Update function defined
	*/
	bool hasUpdateFunction() const;

	/**
	 * @brief Returns true if the behavior has an Init function defined
	*/
	bool hasInitFunction() const;

	/**
	 * @brief Returns true if the behavior has a OnTriggerEnter function defined
	*/
	bool hasTriggerEnterFunction() const;

	/**
	 * @brief Returns true if the behavior has a OnTriggerLeave function defined
	*/
	bool hasTriggerLeaveFunction() const;

	/**
	 * @brief Returns true if the behavior has a OnCollisionEnter function defined
	*/
	bool hasCollisionEnterFunction() const;

	/**
	 * @brief Returns true if the behavior has a OnCollisionLeave function defined
	*/
	bool hasCollisionLeaveFunction() const;

	/**
	 * @brief Invokes Init function on the specified instance
	 * @param instance Object to call Init on
	*/
	void init(MonoObject* instance) const;

	/**
	 * @brief Invokes the update method on all instances of the behavior class
	 * @param instance Instance of the object
	 * @param delta Frame delta value
	*/
	void update(MonoObject* instance, float delta) const;

	/**
	 * @brief Invokes on trigger method on the specified instance, passing the specified trigger object
	 * @param instance Instance of the object for which to invoke
	 * @param trigger Trigger entity
	*/
	void onTriggerEnter(MonoObject* instance, MonoObject* trigger) const;

	/**
	 * @brief Invokes on trigger method on the specified instance, passing the specified trigger object
	 * @param instance Instance of the object for which to invoke
	 * @param trigger Trigger entity
	*/
	void onTriggerLeave(MonoObject* instance, MonoObject* trigger) const;

	/**
	 * @brief Invokes on collision method on the specified instance, passing the specified collision object
	 * @param instance Instance of the object for which to invoke
	 * @param collision Collision entity
	*/
	void onCollisionEnter(MonoObject* instance, MonoObject* collision) const;

	/**
	 * @brief Invokes on collision method on the specified instance, passing the specified collision object
	 * @param instance Instance of the object for which to invoke
	 * @param collision Collision entity
	*/
	void onCollisionLeave(MonoObject* instance, MonoObject* collision) const;

	/**
	 * @brief Creates an instance of this behavior
	*/
	MonoObject* createInstance();

	/**
	 * @brief Returns the fields of the behavior
	*/
	const std::vector<FieldWrapper*>& getFields() const;

	/**
	 * @brief Returns the field wrapper for the specified field
	 * @param fieldName Field name
	 * @returns FieldWrapper instance or nullptr if field doesn't exist
	*/
	FieldWrapper* getField(const std::string& fieldName) const;

	/**
	 * @brief Pair mono instance with entity
	 * @param entity Entity to pair
	 * @param instance MonoObject instance to pair
	*/
	void pair(scene::Entity entity, MonoObject* instance);

	/**
	 * @brief Returns the name of the behavior
	*/
	std::string getName() const;
private:
	/**
	 * @brief Resolves a method from image and class and returns it
	 * @param signature Signature of the method
	 * @return MonoMethod instance or nullptr if not found
	*/
	MonoMethod* resolveMethod(const std::string& signature);

	/**
	 * @brief Resolves all fields of the class
	*/
	void resolveFields();
private:
	MonoImage* m_image = nullptr;
	MonoClass* m_class = nullptr;

	MonoMethod* m_updateMethod = nullptr;
	UpdateThunkFn* m_updateThunk = nullptr;

	MonoMethod* m_initMethod = nullptr;

	MonoMethod* m_triggerEnterMethod = nullptr;
	TriggerThunkFn* m_triggerEnterThunk = nullptr;

	MonoMethod* m_triggerLeaveMethod = nullptr;
	TriggerThunkFn* m_triggerLeaveThunk = nullptr;

	MonoMethod* m_collisionEnterMethod = nullptr;
	TriggerThunkFn* m_collisionEnterThunk = nullptr;

	MonoMethod* m_collisionLeaveMethod = nullptr;
	TriggerThunkFn* m_collisionLeaveThunk = nullptr;

	std::vector<FieldWrapper*> m_fields;

	std::string m_name;
};

}
}
