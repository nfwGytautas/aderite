#pragma once

#include <filesystem>
#include "aderiteeditor/utility/Macros.hpp"
#include "aderiteeditor/shared/BaseComponent.hpp"

ADERITE_EDITOR_COMPONENT_NAMESPACE_BEGIN

/**
 * @brief Asset browser used by aderite. This component allows the user to browse the root asset manager directory.
*/
class AssetBrowser : public shared::BaseComponent {
public:
	enum class FsNodeType {
		UNKNOWN = -1,
		DIRECTORY = 0,
		SCENE = 1,
		SHADER = 2,
		MESH = 3,
		MATERIAL = 4,
		BANK = 5,
	};

	struct FsNode {
		FsNodeType Type;
		std::string Stem;
		std::string Name;
		std::string Extension;
	};

public:
	AssetBrowser();
	virtual ~AssetBrowser();

	// Inherited via BaseComponent
	virtual void render() override;

private:
	/**
	 * @brief Resolve the directories depending on the current dir
	*/
	void resolveFs();

	/**
	 * @brief Deletes an item from the filesystem, be it directory or a simple file
	 * @param path Path to object delete
	*/
	void deleteItem(std::filesystem::path path);

	/**
	 * @brief Renames an item and updates asset manager registry
	 * @param prevName Previous name of the object
	 * @param newName New name of the object
	*/
	void renameItem(const std::string& prevName, const std::string& newName);

private:
	// How many frames to wait before updating
	int m_updateInterval = 15;

	// This is relative to root directory
	std::filesystem::path m_currentDir;

	std::vector<FsNode> m_itNode = {};
	std::vector<std::string> m_pathNodes = {};
};

ADERITE_EDITOR_COMPONENT_NAMESPACE_END
