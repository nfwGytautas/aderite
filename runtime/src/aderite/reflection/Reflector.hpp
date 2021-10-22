#pragma once

#include <map>
#include <string>
#include "aderite/utility/Macros.hpp" 
#include "aderite/reflection/Forward.hpp"
#include "aderite/reflection/RuntimeTypes.hpp"
#include "aderite/reflection/Instancer.hpp"

// Helper macro similar to ADERITE_REFLECTOR_EXPOSE, but this accepts an instance, useful when making many expose calls in a single scope
#define ADERITE_REFLECTOR_EXPOSE_INSTANCE(instance, klass, type) instance->expose<klass>(static_cast<::aderite::reflection::Type>(type), ADERITE_STRINGIZE(klass))

// Helper macro to expose a type with optional name, usefully when having an enum of types
#define ADERITE_REFLECTOR_EXPOSE(klass, type) ADERITE_REFLECTOR_EXPOSE_INSTANCE(::aderite::Engine::getReflector(), klass, type)

namespace aderite {
namespace reflection {

/**
 * @brief Reflector is responsible for the entire aderite reflection system
*/
class Reflector {
public:
	/**
	 * @brief Initializes the reflector
	 * @return True if initialized without error, false otherwise
	*/
	bool init();

	/**
	 * @brief Shutdown reflector
	*/
	void shutdown();

	/**
	 * @brief Expose a type to the reflector
	 * @tparam T Type of the object to expose
	 * @param type Numerical index of the type
	 * @param name Optional name of the type
	*/
	template<typename T>
	void expose(Type type, const std::string& name = "") {
		InstancerBase* instancer = new Instancer<T>();
		this->linkInstancer(type, instancer, name);
	}

	/**
	 * @brief Creates an object if it's reflectable
	 * @tparam T Type of the reflectable
	 * @param type Type ENUM
	 * @return Object instance
	*/
	template<typename T>
	T* reflect(Type type) const {
		InstancerBase* instancer = this->resolveInstancer(type);
		return static_cast<T*>(instancer->create());
	}

	/**
	 * @brief Reflect the name of the type
	 * @param type Type whose name to reflect
	 * @return Name associated with type
	*/
	std::string reflectName(Type type) const;

	/**
	 * @brief Resolves an instancer for a type, asserts if a type doesn't have a type
	 * @param type Type to get instancer for
	 * @return Instancer instance
	*/
	InstancerBase* resolveInstancer(Type type) const;

	ADERITE_DEBUG_SECTION(
		/**
		 * @brief Debug functionality to print what type is linked to what instancer
		*/
		void printInstancers();
	);

private:
	/**
	 * @brief Links a type to an instancer, if a type already has a linked instancer, then
	 * the instancer is overridden and a DEBUG message is logged
	 * @param type Type to link
	 * @param instancer Instancer instance to use (takes ownership)
	*/
	void linkInstancer(Type type, InstancerBase* instancer, const std::string& name);

private:
	Reflector() {}
	friend class Engine;
private:
	std::map<Type, InstancerBase*> m_instancers;
	std::map<Type, std::string> m_names;
};

}
}
