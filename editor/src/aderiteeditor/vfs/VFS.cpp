#include "VFS.hpp"

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "aderiteeditor/vfs/Directory.hpp"
#include "aderiteeditor/vfs/File.hpp"

namespace aderite {
namespace vfs {

void serializeDirectory(YAML::Emitter& emitter, Directory* dir) {
	emitter << YAML::BeginMap; // Directory

	emitter << YAML::Key << "Name" << YAML::Value << dir->getName();
	emitter << YAML::Key << "Directories" << YAML::BeginSeq; // Directories
	for (Directory* d : dir->getDirectories()) {
		serializeDirectory(emitter, d);
	}
	emitter << YAML::EndSeq; // Directories

	emitter << YAML::Key << "Files" << YAML::BeginSeq; // Files
	for (File* f : dir->getFiles()) {
		emitter << YAML::BeginMap; // File
		emitter << YAML::Key << "Name" << YAML::Value << f->getName();
		emitter << YAML::Key << "Content" << YAML::Value << f->getHandle();
		emitter << YAML::EndMap; // File
	}
	emitter << YAML::EndSeq; // Files

	emitter << YAML::EndMap; // Directory
}

void deserializeDirectory(YAML::Node& node, Directory* parent) {
	for (YAML::Node& subdirNode : node["Directories"]) {
		Directory* dir = new Directory(subdirNode["Name"].as<std::string>(), parent);
		deserializeDirectory(subdirNode, dir);
	}

	for (YAML::Node& fileNode : node["Files"]) {
		std::string name = fileNode["Name"].as<std::string>();
		io::SerializableHandle handle = fileNode["Content"].as<io::SerializableHandle>();
		File* file = new File(name, handle, parent);
	}
}

File* searchFile(Directory* dir, io::SerializableHandle handle) {
	for (File* f : dir->getFiles()) {
		if (f->getHandle() == handle) {
			return f;
		}
	}

	// Recursive search directories
	for (Directory* d : dir->getDirectories()) {
		File* f = searchFile(d, handle);
		if (f != nullptr) {
			return f;
		}
	}
}

VFS::VFS() 
	: m_root(new Directory("/"))
{}

VFS::~VFS() {
	delete m_root;
}

std::vector<File*> VFS::findFiles(const std::string& name) const {
	return std::vector<File*>();
}

File* VFS::getFile(const std::string& path) const {
	std::filesystem::path p(path);
	Directory* dir = this->getDirectory(p.parent_path().string());

	if (dir == nullptr) {
		return nullptr;
	}

	return dir->getFile(p.filename().string());
}

File* VFS::getFile(io::SerializableHandle handle) const {
	return searchFile(m_root, handle);
}

Directory* VFS::getDirectory(const std::string& path) const {
	std::filesystem::path p(path);
	Directory* cd = m_root;

	for (auto& part : p) {
		cd = cd->getDirectory(part.string());

		if (cd == nullptr) {
			return nullptr;
		}
	}

	return cd;
}

Directory* VFS::getRoot() const {
	return m_root;
}

void VFS::move(Directory* target, File* file) {
	if (target->hasFile(file->getName())) {
		this->remove(target->getFile(file->getName()));
	}
	
	target->m_files.push_back(file);
}

void VFS::move(Directory* target, Directory* dir) {
	if (target->hasDirectory(dir->getName())) {
		this->remove(target->getDirectory(dir->getName()));
	}

	dir->getParent()->m_directories.erase(std::find(dir->getParent()->m_directories.begin(), dir->getParent()->m_directories.end(), dir));
	dir->m_parent = target;
	target->m_directories.push_back(dir);
}

void VFS::remove(File* file) {
	file->getParent()->m_files.erase(
		std::find(
		file->getParent()->m_files.begin(), 
		file->getParent()->m_files.end(),
		file)
	);
	file->m_parent = nullptr;
	delete file;
	// TODO: Remove from serializer
}

void VFS::remove(Directory* dir) {
	dir->getParent()->m_directories.erase(
		std::find(
		dir->getParent()->m_directories.begin(),
		dir->getParent()->m_directories.end(),
		dir)
	);
	dir->m_parent = nullptr;
	delete dir;
	// TODO: Remove from serializer
	// TODO: Remove all files
}

void VFS::rename(File* file, const std::string& newName) {
	if (file->getParent()->hasFile(newName)) {
		return;
	}

	file->m_name = newName;
}

void VFS::rename(Directory* dir, const std::string& newName) {
	if (dir->getParent()->hasDirectory(newName)) {
		return;
	}

	dir->m_name = newName;
}

void VFS::save(std::filesystem::path root) const {
	YAML::Emitter emitter;

	emitter << YAML::BeginMap; // Root
	emitter << YAML::Key << "Root";
	serializeDirectory(emitter, m_root);
	emitter << YAML::EndMap;

	std::ofstream of(root / "VFS.avfs");
	of << emitter.c_str();
}

VFS* VFS::load(std::filesystem::path root) {
	VFS* vfs = new VFS();
	YAML::Node node = YAML::LoadFile((root / "VFS.avfs").string());

	if (node["Root"]) {
		deserializeDirectory(node["Root"], vfs->m_root);
	}

	return vfs;
}

}
}
