#pragma once

#include <ostream>
#include <sstream>

#include "aderite/asset/MaterialTypeAsset.hpp"

#include "aderiteeditor/asset/property/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"
#include "aderiteeditor/compiler/GraphEvaluator.hpp"
#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace compiler {

/**
 * @brief Object used to create a shader from node graph
 */
class ShaderEvaluator : public GraphEvaluator {
public:
    struct Function {
        std::string ReturnType;
        std::string Name;
        std::string Arguments;
        std::stringstream Body;
    };

    enum class ShaderType {
        UNKNOWN = -1,
        FRAGMENT = 0,
        VERTEX = 1,
    };

public:
    ShaderEvaluator(ShaderType type, asset::MaterialTypeAsset* material);

    /**
     * @brief Returns an access variable for accessing object properties
     * @param prop Property instance
     * @return Variable name
     */
    EvaluatorValue getProperty(const asset::Property* prop);

    /**
     * @brief Returns an access variable for accessing object properties
     * @param samp Sampler instance
     * @return Variable name
     */
    EvaluatorValue getSampler(const asset::Sampler* samp);

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
     * @brief Write shader to the specified stream
     * @param stream Stream to write to
     */
    void write(std::ostream& stream);

    /**
     * @brief Returns the type of the writer
     */
    ShaderType getType() const;

private:
    // Parts
    void writeInputsOutputs(std::ostream& of);
    void writeGenerationComment(std::ostream& of);
    void writeLibraries(std::ostream& of);
    void writeConstants(std::ostream& of);
    void writeFunctions(std::ostream& of);

    // Sub parts
    void writeScope(std::ostream& of, Function* scope);

    // Helpers
    const char* getPrefix() const;
    const char* getFullName() const;
    Function createMainScope() const;

    std::string getValue(EvaluatorValue value) const;
    EvaluatorValue createValue();
    EvaluatorValue createValue(const std::string& value);

private:
    ShaderType m_type = ShaderType::UNKNOWN;
    asset::MaterialTypeAsset* m_material = nullptr;

    std::vector<Function> m_functions;
    Function* m_currentScope = nullptr;
    Function* m_mainScope = nullptr;

    std::vector<std::string> m_values;
};

} // namespace compiler
} // namespace aderite
