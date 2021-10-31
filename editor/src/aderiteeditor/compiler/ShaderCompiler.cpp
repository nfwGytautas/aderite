#include "ShaderCompiler.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

#include "aderite/utility/Log.hpp"

namespace aderite {
namespace compiler {

ShaderCompiler::ShaderCompiler(const std::filesystem::path vertex, const std::filesystem::path fragment,
                               const std::filesystem::path varying) :
    m_vertex(vertex),
    m_fragment(fragment),
    m_varying(varying) {}

bool ShaderCompiler::compile() const {
    // TODO: Merge bgfx compiler into here
    // TODO: Replace with lib call
    LOG_WARN("USING SYSTEM FOR COMPILING");
#pragma message("USING SYSTEM FOR COMPILING SHADERS")

    LOG_TRACE("Compiling shaders");

    // Create commands
    std::stringstream fCommand;
    fCommand << "tools\\shadercRelease.exe -f ";
    fCommand << m_fragment.string();
    fCommand << " -o " << this->getFragmentBinPath().string();
    fCommand << " --platform windows --verbose ";
    fCommand << " --varyingdef " << m_varying.string();
    fCommand << " --type fragment --profile ps_5_0";

    std::stringstream vCommand;
    vCommand << "tools\\shadercRelease.exe -f ";
    vCommand << m_vertex.string();
    vCommand << " -o " << this->getVertexBinPath().string();
    vCommand << " --platform windows --verbose ";
    vCommand << " --varyingdef " << m_varying.string();
    vCommand << " --type vertex --profile vs_5_0";

    // Compile
    LOG_TRACE("Running {0}", vCommand.str().c_str());
    system(vCommand.str().c_str());
    LOG_TRACE("Running {0}", fCommand.str().c_str());
    system(fCommand.str().c_str());

    return true;
}

std::filesystem::path ShaderCompiler::getFragmentBinPath() const {
    return m_fragment.parent_path() / m_fragment.filename().replace_extension(".fs.bin");
}

std::filesystem::path ShaderCompiler::getVertexBinPath() const {
    return m_vertex.parent_path() / m_vertex.filename().replace_extension(".vs.bin");
}

} // namespace compiler
} // namespace aderite
