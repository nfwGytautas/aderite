#pragma once

#include <string>
#include "aderite/utility/Macros.hpp"
#include "aderite/interfaces/ISerializable.hpp"
#include "aderite/interfaces/ILoadable.hpp"

ADERITE_ASSET_NAMESPACE_BEGIN

/**
 * @brief Possible asset groups a single asset can have multiple groups
*/
enum class AssetGroup : size_t {
	// Is the asset a system type asset
	SYSTEMIC = 0,

	// Does the asset depend on some external resource from Raw/ directory
	DEPENDS_ON_RAW = 1,
};

/**
 * @brief The asset type
*/
enum class AssetType : size_t {
	SHADER		= 0,
	SCENE		= 1,
	MATERIAL	= 2,
	MESH		= 3,
};

/**
 * @brief Asset base generic class
*/
class Asset : public interfaces::ISerializable, public interfaces::ILoadable {
public:
	virtual ~Asset() {}

	std::string getName() const {
		return p_name;
	}

	void setName(const std::string& name) {
		p_name = name;
	}

	/**
	 * @brief Returns the asset type
	*/
	virtual AssetType type() const = 0;

	/**
	 * @brief Returns true if the asset is in group
	*/
	virtual bool isInGroup(AssetGroup group) const = 0;

	/**
	 * @brief Returns the hash of the asset
	*/
	virtual size_t hash() const = 0;
protected:
	Asset(const std::string& name)
		: p_name(name)
	{}

	friend class AssetManager;
protected:
	std::string p_name;
};

ADERITE_ASSET_NAMESPACE_END
