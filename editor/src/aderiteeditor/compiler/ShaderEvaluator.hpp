#pragma once

#include <sstream>
#include <fstream>
#include "aderite/asset/MaterialTypeAsset.hpp"
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"
#include "aderiteeditor/asset/property/Forward.hpp"

ADERITE_EDITOR_COMPILER_NAMESPACE_BEGIN

/**
 * @brief Object used to create a shader from node graph
*/
class ShaderEvaluator : public GraphEvaluator
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
	ShaderEvaluator(ShaderType type, const std::string& name);

	/**
	 * @brief Returns an access variable for accessing object properties
	 * @param material Material intance
	 * @param prop Property instance
	 * @return Variable name
	*/
	EvaluatorValue getProperty(const asset::MaterialTypeAsset* material, const asset::Property* prop);

	/**
	 * @brief Adds a 2D sampling instruction to the current scope
	 * @param texture Texture to sample, (must be in scope)
	 * @return Variable where result is stored
	*/
	EvaluatorValue add2DSamplingInstruction(const EvaluatorValue& texture);

	/**
	 * @brief Adds an addition instruction to the current scope
	 * @param type Type of the values
	 * @param lhs Left hand side variable
	 * @param rhs Right hand side variable
	 * @return Variable where result is stored
	*/
	EvaluatorValue addAddInstruction(const std::string& type, const EvaluatorValue& lhs, const EvaluatorValue& rhs);

	/**
	 * @brief Adds a fragment color assign instruction to the current scope (must be main)
	 * @param value Value to assign
	*/
	void addFragmentColorInstruction(const EvaluatorValue& value);

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

	std::string getValue(EvaluatorValue value) const;
	EvaluatorValue createValue();
	EvaluatorValue createValue(const std::string& value);
private:
	ShaderType m_type = ShaderType::UNKNOWN;
	std::string m_name;

	std::vector<Function> m_functions;
	Function* m_currentScope = nullptr;
	Function* m_mainScope = nullptr;

	std::vector<std::string> m_values;
};

ADERITE_EDITOR_COMPILER_NAMESPACE_END
