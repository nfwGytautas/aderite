#include "VFS.hpp"

#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "aderiteeditor/vfs/Directory.hpp"
#include "aderiteeditor/vfs/File.hpp"

namespace aderite {
namespace vfs {

void serializeDirectory(YAML::Emitter& emitter, Directory* dir) {
	emitter << YAML::Key << "Directory" << YAML::BeginMap;
	emitter << YAML::Key << "Name" << YAML::Value << dir->getName();

	emitter << YAML::Key << "Directories" << YAML::BeginSeq;
	for (Directory* d : dir->getDirectories()) {
		serializeDirectory(emitter, d);
	}
	emitter << YAML::EndSeq;

	emitter << YAML::Key << "Files" << YAML::BeginSeq;
	for (File* f : dir->getFiles()) {
		emitter << YAML::Key << "File" << YAML::BeginMap;
		emitter << YAML::Key << "Name" << YAML::Value << f->getName();
		emitter << YAML::Key << "Content" << YAML::Value << f->getHandle();
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;

	emitter << YAML::EndMap;
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

VFS::VFS() {}

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

	target->m_directories.push_back(dir);
}

void VFS::remove(File* file) {
	// TODO: Remove from serializer
}

void VFS::remove(Directory* dir) {
	// TODO: Remove from serializer
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

	emitter << YAML::Key << "VFS" << YAML::BeginMap;
	serializeDirectory(emitter, m_root);
	emitter << YAML::EndMap;
}

VFS* VFS::load(std::filesystem::path root) {
	VFS* vfs = new VFS();



	return vfs;
}

}
}
