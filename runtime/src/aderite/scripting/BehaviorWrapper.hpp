#pragma once

#include <string>
#include <mono/jit/jit.h>

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

	/**
	 * @brief Returns true if the behavior has an Update function defined
	*/
	bool hasUpdateFunction() const;

	/**
	 * @brief Invokes the update method if the behavior class
	 * @param instance Object instance
	 * @param delta Delta value
	*/
	void invokeUpdate(MonoObject* instance, float delta) const;

	/**
	 * @brief Creates an instance of this behavior
	*/
	MonoObject* createInstance();
private:
	/**
	 * @brief Resolves a method from image and class and returns it
	 * @param signature Signature of the method
	 * @return MonoMethod instance or nullptr if not found
	*/
	MonoMethod* resolveMethod(const std::string& signature);
private:
	MonoImage* m_image = nullptr;
	MonoClass* m_class = nullptr;

	MonoMethod* m_updateMethod = nullptr;
	UpdateThunkFn* m_updateThunk = nullptr;
};

}
}
