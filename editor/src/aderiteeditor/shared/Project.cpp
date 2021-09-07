#include "Project.hpp"

#include <fstream>
#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/scene/SceneManager.hpp"
#include "aderite/scene/Scene.hpp"

// Previous versions:
//	- 2021_07_31r1

constexpr const char* current_version = "2021_07_31r1";

ADERITE_EDITOR_SHARED_NAMESPACE_BEGIN

Project::Project(const std::string& dir, const std::string& name)
	: m_directory(dir), m_name(name)
{}

Project::~Project() {
}

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

	if (::aderite::Engine::getSceneManager()->getCurrentScene()) {
		out << YAML::Key << "ActiveScene" << YAML::Value << ::aderite::Engine::getSceneManager()->getCurrentScene()->getName();
		m_activeScene = ::aderite::Engine::getSceneManager()->getCurrentScene()->getName();
	}

	out << YAML::Key << "MasterBanksDirectory" << YAML::Value << m_masterBanksDir;

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

	if (data["ActiveScene"]) {
		p->m_activeScene = data["ActiveScene"].as<std::string>();
	}

	if (data["MasterBanksDirectory"]) {
		p->m_masterBanksDir = data["MasterBanksDirectory"].as<std::string>();
	}

	return p;
}

std::string Project::getName() const {
	return m_name;
}

std::filesystem::path Project::getRootDir() const {
	return m_directory;
}

std::string Project::getActiveScene() const {
	return m_activeScene;
}

std::string Project::getMasterBanksDir() const {
	return m_masterBanksDir;
}

void Project::setMasterBanksDir(const std::string& masterBanksDir) {
	m_masterBanksDir = masterBanksDir;
}

void Project::validate() {
	if (::aderite::Engine::getSceneManager()->getCurrentScene() == nullptr && !m_activeScene.empty()) {
		save();
	}
	else if (::aderite::Engine::getSceneManager()->getCurrentScene()->getName() != m_activeScene) {
		save();
	}
}

ADERITE_EDITOR_SHARED_NAMESPACE_END
