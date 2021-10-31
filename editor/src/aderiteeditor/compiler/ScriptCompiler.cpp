#include "ScriptCompiler.hpp"
#include <filesystem>
#include <sstream>

#include "aderite/Aderite.hpp"
#include "aderite/io/FileHandler.hpp"
#include "aderite/utility/Log.hpp"

#include "aderiteeditor/shared/Project.hpp"
#include "aderiteeditor/shared/State.hpp"

namespace aderite {
namespace compiler {

void ScriptCompiler::compile() {
    const std::filesystem::path scriptRoot = editor::State::Project->getRootDir() / "Scripts/";
    const std::filesystem::path codeChunkPath = ::aderite::Engine::getFileHandler()->pathToReserved(io::FileHandler::Reserved::GameCode);

    LOG_WARN("USING SYSTEM FOR COMPILING");
#pragma message("USING SYSTEM FOR COMPILING SCRIPTS")

    LOG_TRACE("Compiling scripts");

    if (std::filesystem::exists(codeChunkPath)) {
        LOG_TRACE("Cleaning old code chunk");
        std::filesystem::remove(codeChunkPath);
    }

    // Create command
    std::stringstream sCommand;
    sCommand << "cmd /c \"C:\\Program Files\\Mono\\bin\\csc.bat\""; // Compiler
    sCommand << " -t:library";                                      // DLL
    sCommand << " -out:" << codeChunkPath.string();                 // Output to game code chunk

    // Link scriptlib
    sCommand << " -lib:" << codeChunkPath.parent_path().string();
    sCommand << " -reference:_" << std::to_string(aderite::io::FileHandler::Reserved::ScriptLibCode) << ".data";

    // Sources
    sCommand << " -recurse:" << scriptRoot.string() << "*.cs";

    // Execute command
    LOG_TRACE("Executing {0}", sCommand.str().c_str());
    system(sCommand.str().c_str());

    LOG_TRACE("Compiled to {0}", codeChunkPath.string());
}

} // namespace compiler
} // namespace aderite
