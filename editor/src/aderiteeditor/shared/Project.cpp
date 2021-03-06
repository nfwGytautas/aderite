#include "Project.hpp"
#include <filesystem>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/scene/Scene.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/utility/Log.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

namespace aderite {
namespace editor {

Project::Project(const std::string& dir, const std::string& name) : m_directory(dir), m_name(name) {}

Project::~Project() {}

bool Project::save() {
    std::filesystem::path root = std::filesystem::path(m_directory);

    // Check if directory exists if not then create one
    if (!std::filesystem::is_directory(root) || !std::filesystem::exists(root)) {
        std::filesystem::create_directory(root);
    }

    YAML::Emitter out;
    out << YAML::BeginMap; // Root

    // Common
    out << YAML::Key << "Version" << YAML::Value << current_version;
    out << YAML::Key << "Name" << YAML::Value << m_name;
    out << YAML::Key << "Type" << YAML::Value << "Project";

    out << YAML::Key << "ActiveScene" << YAML::Value;
    if (::aderite::Engine::getSceneManager()->getCurrentScene()) {
        out << ::aderite::Engine::getSceneManager()->getCurrentScene()->getHandle();
        m_activeScene = ::aderite::Engine::getSceneManager()->getCurrentScene()->getHandle();
    } else {
        out << YAML::Null;
    }

    out << YAML::Key << "FMODProject" << YAML::Value << m_fmodProjectDirectory.string();

    out << YAML::EndMap; // Root

    std::ofstream fout(root.append(m_name + ".aproj"));
    fout << out.c_str();

    return true;
}

Project* Project::load(const std::string& path) {
    YAML::Node data = YAML::LoadFile(path);

    // Check version
    if (!data["Version"]) {
        LOG_ERROR("Loading scene from {0} failed because there is no version information", path);
        return nullptr;
    }

    // Check type
    if (!data["Type"]) {
        LOG_ERROR("Loading scene from {0} failed because no type information was given", path);
        return nullptr;
    }

    if (data["Type"].as<std::string>() != "Project") {
        LOG_ERROR("Trying to load asset of type {0} as a project. File {1}", data["Type"].as<std::string>(), path);
        return nullptr;
    }

    std::filesystem::path fs_path = std::filesystem::path(path);
    Project* p = new Project(fs_path.parent_path().string(), fs_path.stem().string());

    if (!data["ActiveScene"].IsNull()) {
        p->m_activeScene = data["ActiveScene"].as<io::SerializableHandle>();
    } else {
        p->m_activeScene = c_InvalidHandle;
    }

    if (data["FMODProject"]) {
        p->m_fmodProjectDirectory = data["FMODProject"].as<std::string>();
    }

    return p;
}

std::string Project::getName() const {
    return m_name;
}

std::filesystem::path Project::getRootDir() const {
    return m_directory;
}

std::filesystem::path Project::getFmodProjectDir() const {
    return m_fmodProjectDirectory;
}

void Project::linkFmodProject(std::filesystem::path project) {
    if (!std::filesystem::exists(project)) {
        LOG_WARN("Invalid project directory");
        return;
    }

    m_fmodProjectDirectory = project;
}

io::SerializableHandle Project::getActiveScene() const {
    return m_activeScene;
}

void Project::validate() {
    if (::aderite::Engine::getSceneManager()->getCurrentScene() == nullptr && m_activeScene != c_InvalidHandle) {
        save();
    } else if (::aderite::Engine::getSceneManager()->getCurrentScene()->getHandle() != m_activeScene) {
        save();
    }
}

} // namespace editor
} // namespace aderite
