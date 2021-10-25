#pragma once

#include <string>
#include <vector>
#include <mono/jit/jit.h>

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

}
}
