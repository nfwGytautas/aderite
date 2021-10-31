#pragma once

#include <filesystem>

namespace aderite {
namespace compiler {

/**
 * @brief Shader compiler utility class, for compiling shader sources into binaries
 */
class ShaderCompiler {
public:
    ShaderCompiler(const std::filesystem::path vertex, const std::filesystem::path fragment, const std::filesystem::path varying);

    /**
     * @brief Compiles the shaders
     * @return True if compiled successfully, false otherwise
     */
    bool compile() const;

    /**
     * @brief Returns the fragment binary file path
     */
    std::filesystem::path getFragmentBinPath() const;

    /**
     * @brief Returns the vertex binary file path
     */
    std::filesystem::path getVertexBinPath() const;

private:
    const std::filesystem::path m_vertex;
    const std::filesystem::path m_fragment;
    const std::filesystem::path m_varying;
};

} // namespace compiler
} // namespace aderite
