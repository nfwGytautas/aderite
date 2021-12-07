#pragma once

#include <ostream>
#include <sstream>

#include "aderite/asset/MaterialTypeAsset.hpp"

#include "aderiteeditor/asset/property/Forward.hpp"
#include "aderiteeditor/compiler/Forward.hpp"

namespace aderite {
namespace compiler {

/**
 * @brief Object used to create a shader from node graph
 */
class ShaderEvaluator {
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
     * @brief Adds a comment to the current scope
     * @param comment Comment to add
    */
    void addComment(const std::string& comment);

    void getMaterialField(const std::string& field, const std::string& storeIn, const std::string& type);

    /**
     * @brief Returns an access variable for accessing object properties
     * @param prop Property instance
     * @param storeIn Variable to store result in
     */
    void getProperty(const asset::Property* prop, const std::string& storeIn);

    /**
     * @brief Returns an access variable for accessing vertex attributes
     * @param attr Attribute name
     * @param storeIn Variable to store result in
     */
    void getAttribute(const std::string& attr, const std::string& storeIn, const std::string& type);

    /**
     * @brief Returns an access variable for accessing object properties
     * @param samp Sampler instance
     * @param storeIn Variable to store result in
     */
    void getSampler(const asset::Sampler* samp, const std::string& storeIn);

    /**
     * @brief Adds a 2D sampling instruction to the current scope
     * @param texture Texture to sample, (must be in scope)
     * @param uv UV coordinates to sample from
     * @param storeIn Variable to store result in
     */
    void add2DSamplingInstruction(const std::string& texture, const std::string& uv, const std::string& storeIn);

    /**
     * @brief Adds an addition instruction to the current scope
     * @param type Type of the values
     * @param lhs Left hand side variable
     * @param rhs Right hand side variable
     * @param storeIn Variable to store result in
     */
    void addAddInstruction(const std::string& type, const std::string& lhs, const std::string& rhs, const std::string& storeIn);

    /**
     * @brief Adds a fragment color assign instruction to the current scope (must be main)
     * @param value Value to assign
     */
    void addFragmentColorInstruction(const std::string& value);

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

private:
    ShaderType m_type = ShaderType::UNKNOWN;
    asset::MaterialTypeAsset* m_material = nullptr;

    std::vector<Function> m_functions;
    Function* m_currentScope = nullptr;
    Function* m_mainScope = nullptr;
};

} // namespace compiler
} // namespace aderite
