#pragma once

#include <string>
#include <vector>
#include <mono/jit/jit.h>
#include "aderite/scripting/Forward.hpp"

namespace aderite {
namespace scripting {

/**
 * @brief Special utility class for analyzing and using behavior marked classes
*/
class BehaviorWrapper final {
private:
	using UpdateThunkFn = void(MonoObject*, float delta, MonoException**);

public:
	BehaviorWrapper(MonoImage* image, MonoClass* klass);
	~BehaviorWrapper();

	/**
	 * @brief Returns true if the behavior has an Update function defined
	*/
	bool hasUpdateFunction() const;

	/**
	 * @brief Invokes the update method on all instances of the behavior class
	 * @param instance Instance of the object
	 * @param delta Frame delta value
	*/
	void update(MonoObject* instance, float delta) const;

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

	std::vector<FieldWrapper*> m_fields;

	std::string m_name;
};

}
}
