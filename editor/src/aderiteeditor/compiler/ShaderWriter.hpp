#pragma once

#include <sstream>
#include <fstream>
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

/**
 * @brief Object used to create a shader from node graph
*/
class ShaderWriter
{
public:
	struct Function {
		std::string ReturnType;
		std::string Name;
		std::string Arguments;
		std::stringstream Body;
	};

	enum class ShaderType {
		UNKNOWN		= -1,
		FRAGMENT	=  0,
		VERTEX		=  1,
	};
public:
	ShaderWriter(ShaderType type, const std::string& name);

	/**
	 * @brief Returns an access variable for accessing object properties
	 * @param prop Property name
	 * @return Variable name
	*/
	Variable getProperty(const std::string& prop) const;

	/**
	 * @brief Adds a 2D sampling instruction to the current scope
	 * @param texture Texture to sample, (must be in scope)
	 * @return Variable where result is stored
	*/
	Variable add2DSamplingInstruction(const Variable& texture);

	/**
	 * @brief Adds an addition instruction to the current scope
	 * @param type Type of the values
	 * @param lhs Left hand side variable
	 * @param rhs Right hand side variable
	 * @return Variable where result is stored
	*/
	Variable addAddInstruction(const std::string& type, const Variable& lhs, const Variable& rhs);

	/**
	 * @brief Adds a fragment color assign instruction to the current scope (must be main)
	 * @param value Value to assign
	*/
	void addFragmentColorInstruction(const Variable& value);

	/**
	 * @brief Write shader to the raw directory, the name will the name of the type + writer + ".sc"
	*/
	void writeToFile();

	/**
	 * @brief Returns the type of the writer
	*/
	ShaderType getType() const;
private:
	// Parts
	void writeInputsOutputs(std::ofstream& of);
	void writeGenerationComment(std::ofstream& of);
	void writeLibraries(std::ofstream& of);
	void writeConstants(std::ofstream& of);
	void writeFunctions(std::ofstream& of);

	// Sub parts
	void writeScope(std::ofstream& of, Function* scope);

	// Helpers
	const char* getPrefix() const;
	const char* getFullName() const;
	Function createMainScope() const;
private:
	ShaderType m_type = ShaderType::UNKNOWN;
	std::string m_name;

	std::vector<Function> m_functions;
	Function* m_currentScope = nullptr;
	Function* m_mainScope = nullptr;
};

ADERITE_EDITOR_COMPILER_NAMESPACE_END
