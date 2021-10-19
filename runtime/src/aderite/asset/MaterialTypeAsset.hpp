#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/asset/property/Forward.hpp"
#include "aderite/asset/property/Property.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material type asset implementation
*/
class MaterialTypeAsset : public io::SerializableObject, public io::ILoadable {
public:
	using Properties = std::vector<prop::Property*>;

	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		io::LoadableHandle Vertex;
		io::LoadableHandle Fragment;

		std::string Name;

		size_t ElementCount; // Computed from properties
		size_t v4Count;
		Properties Properties;
	};
public:
	~MaterialTypeAsset();

	// Inherited via ILoadable
	virtual void load(const io::Loader* loader) override;
	virtual void unload() override;

	// Inherited via SerializableObject
	virtual io::SerializableType getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(const io::Serializer* serializer, const YAML::Node& data) override;

	/**
	 * @brief Recalculates property offsets
	*/
	void recalculate();

	/**
	 * @brief Returns the size in bytes
	*/
	size_t getSizeInBytes() const;

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

	bgfx::ProgramHandle getShaderHandle() const {
		return m_handle;
	}

	bgfx::UniformHandle getUniformHandle() const {
		return m_uniform;
	}

	std::unordered_map<std::string, bgfx::UniformHandle> getSamplers() const {
		return m_samplers;
	}
private:
	// Shader
	bgfx::ProgramHandle m_handle = BGFX_INVALID_HANDLE;

	// Material properties
	bgfx::UniformHandle m_uniform = BGFX_INVALID_HANDLE;
	std::unordered_map<std::string, bgfx::UniformHandle> m_samplers;

	fields m_info = {};
};

}
}
