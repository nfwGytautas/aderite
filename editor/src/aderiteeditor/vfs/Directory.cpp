#include "Directory.hpp"

#include "aderiteeditor/vfs/File.hpp"

namespace aderite {
namespace vfs {

Directory::Directory(const std::string& name, Directory* parent) 
	: m_name(name), m_parent(parent)
{
	if (m_parent != nullptr) {
		m_parent->m_directories.push_back(this);
	}
}

Directory::~Directory() {
	for (Directory* dir : m_directories) {
		delete dir;
	}

	for (File* f : m_files) {
		delete f;
	}
}

Directory* Directory::getParent() const {
	return m_parent;
}

std::string Directory::getName() const {
	return m_name;
}

File* Directory::getFile(const std::string& name) {
	auto it = std::find_if(m_files.begin(), m_files.end(), [name](File* f) {
		return f->getName() == name;
	});

	if (it == m_files.end()) {
		return nullptr;
	}

	return *it;
}

bool Directory::hasFile(const std::string& name) const {
	auto it = std::find_if(m_files.begin(), m_files.end(), [name](File* f) {
		return f->getName() == name;
	});

	if (it == m_files.end()) {
		return false;
	}

	return true;
}

Directory* Directory::getDirectory(const std::string& name) {
	auto it = std::find_if(m_directories.begin(), m_directories.end(), [name](Directory* d) {
		return d->getName() == name;
	});

	if (it == m_directories.end()) {
		return nullptr;
	}

	return *it;
}

bool Directory::hasDirectory(const std::string& name) const {
	auto it = std::find_if(m_directories.begin(), m_directories.end(), [name](Directory* d) {
		return d->getName() == name;
	});

	if (it == m_directories.end()) {
		return false;
	}

	return true;
}

Directory* Directory::createDirectory(const std::string& name) {
	Directory* d = new Directory(name, this);
	return d;
}

const std::vector<Directory*> Directory::getDirectories() const {
	return m_directories;
}

const std::vector<File*> Directory::getFiles() const {
	return m_files;
}
std::string Directory::getPath() const {
	if (m_parent == nullptr) {
		return "/";
	}

	return m_parent->getPath() + "/" + m_name;
}
}
}
