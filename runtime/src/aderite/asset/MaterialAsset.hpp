#pragma once

#include <string>
#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/asset/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material asset implementation
*/
class MaterialAsset : public io::SerializableObject, public io::ILoadable {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		MaterialTypeAsset* Type; // Material type
		std::vector<TextureAsset*> Samplers;
	};
public:
	~MaterialAsset();

	// Inherited via ILoadable
	virtual void load(const io::Loader* loader) override;
	virtual void unload() override;
	virtual bool needsLoading() override;

	// Inherited via SerializableObject
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

	/**
	 * @brief Sets the type of the material
	 * @param type New type
	*/
	void setType(MaterialTypeAsset* type);

	/**
	 * @brief Returns the info of shader fields
	*/
	fields getFields() const {
		return m_info;
	}

	/**
	 * @brief Returns mutable field structure
	*/
	fields& getFieldsMutable() {
		return m_info;
	}

	float* getPropertyData() const {
		return m_udata;
	}

	std::vector<std::pair<bgfx::UniformHandle, bgfx::TextureHandle>> getSamplerData() const;
private:
	fields m_info = {};
	float* m_udata = nullptr; // Data passed to material uniform
};

}
}
