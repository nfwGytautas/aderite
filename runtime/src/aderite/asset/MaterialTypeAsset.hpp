#pragma once

#include <vector>
#include <bgfx/bgfx.h>
#include "aderite/utility/Macros.hpp"
#include "aderite/io/SerializableObject.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/asset/Forward.hpp"

namespace aderite {
namespace asset {

/**
 * @brief Material type asset, is a collection of a shader, data uniform and samplers
*/
class MaterialTypeAsset : public io::SerializableObject, public io::ILoadable {
public:
	/**
	 * @brief Editable fields of the asset, this information is stored inside the asset file
	*/
	struct fields {
		size_t Size; // Number of vec4 components
		size_t NumSamplers;
	};
public:
	virtual ~MaterialTypeAsset();

	/**
	 * @brief Returns true if the type is valid, false otherwise
	*/
	bool isValid() const;

	// Inherited via ILoadable
	virtual void load(const io::Loader* loader) override;
	virtual void unload() override;

	// Inherited via SerializableObject
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;

	bgfx::ProgramHandle getShaderHandle() const {
		return m_shaderHandle;
	}

	bgfx::UniformHandle getUniformHandle() const {
		return m_uniformHandle;
	}

	bgfx::UniformHandle getSampler(size_t idx) const {
		return m_samplers[idx];
	}

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
private:
	// Shader
	bgfx::ProgramHandle m_shaderHandle = BGFX_INVALID_HANDLE;

	// Material properties
	bgfx::UniformHandle m_uniformHandle = BGFX_INVALID_HANDLE;
	std::vector<bgfx::UniformHandle> m_samplers;

	fields m_info = {};
};

}
}
