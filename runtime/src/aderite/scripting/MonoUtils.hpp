#pragma once

#include <string>
#include <vector>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

namespace aderite {
namespace scripting {

/**
 * @brief Utility struct for generating method parameter
*/
struct MethodParameter {
	const std::string Type;
};

/**
 * @brief Utility struct for generating method signatures to pass to mono runtime
*/
struct MethodSignature {
	const std::string Namespace = "";
	const std::string Class = "";
	const std::string Name = "";
	const std::vector<MethodParameter> Parameters;

	const bool IsStatic = false;

	/**
	 * @brief Returns the string representation of the method
	 * @return 
	*/
	const std::string value() const;
};

/**
 * @brief Logs all methods that a class has
 * @param klass Class whose methods to print
*/
void printClassMethods(MonoClass* klass);

/**
 * @brief Unboxes object to a C++ data pointer, this must be a value type
 * @param object Object to unbox
 * @returns Value type pointer
*/
void* unbox(MonoObject* object);

}
}
