#pragma once

#include <string>
#include <vector>

#include <mono/jit/jit.h>

#include "aderite/asset/Forward.hpp"
#include "aderite/audio/Forward.hpp"

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

/**
 * @brief Converts a MonoObject(string object) to std::string
 * @param object Object to convert
 * @return std::string instance
 */
std::string toString(MonoObject* object);

/**
 * @brief Extract mesh field from mono object
 * @param object Object to extract from
 * @param mesh Pointer where to store value
 */
void extract(MonoObject* object, asset::MeshAsset*& mesh);

/**
 * @brief Extract material field from mono object
 * @param object Object to extract from
 * @param material Pointer where to store value
 */
void extract(MonoObject* object, asset::MaterialAsset*& material);

/**
 * @brief Extract audio field from mono object
 * @param object Object to extract from
 * @param audio Pointer where to store value
 */
void extract(MonoObject* object, asset::AudioAsset*& audio);

/**
 * @brief Extract source field from mono object
 * @param object Object to extract from
 * @param source Pointer where to store value
 */
void extract(MonoObject* object, audio::AudioSource*& source);

} // namespace scripting
} // namespace aderite
