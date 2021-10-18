#include "MeshAsset.hpp"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "aderite/Aderite.hpp"
#include "aderite/utility/Log.hpp"
#include "aderite/utility/Macros.hpp"
#include "aderite/io/Loader.hpp"
#include "aderite/io/RuntimeSerializables.hpp"

namespace aderite {
namespace asset {

MeshAsset::~MeshAsset() {
	// TODO: Check for either handle to be valid
	if (bgfx::isValid(m_vbh)) {
		LOG_WARN("Deleting a loaded mesh asset {0}", getHandle());
	}
}

void MeshAsset::load(const io::Loader* loader) {
	// Create layout
	bgfx::VertexLayout layout;
	layout.begin();
	layout.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float);
	layout.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float);
	layout.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
	layout.end();

	// Create handles
	if (m_info.IsStatic) {
		io::Loader::MeshLoadResult result = loader->loadMesh(m_info.DataFile);
		ADERITE_DYNAMIC_ASSERT(!result.Error.empty(), "Failed to load mesh");

		auto& positionData = result.Vertices;
		auto& indicesData = result.Indices;
		m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(positionData.data(), sizeof(float) * positionData.size()), layout);
		m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indicesData.data(), sizeof(unsigned int) * indicesData.size()), BGFX_BUFFER_INDEX32);
	}
	else {
		LOG_ERROR("Unimplemented dynamic mesh");
		return;
	}
}

void MeshAsset::unload() {
	if (bgfx::isValid(m_vbh)) {
		bgfx::destroy(m_vbh);
		m_vbh = BGFX_INVALID_HANDLE;
	}

	if (bgfx::isValid(m_ibh)) {
		bgfx::destroy(m_ibh);
		m_ibh = BGFX_INVALID_HANDLE;
	}
}

io::SerializableType MeshAsset::getType() {
	return static_cast<io::SerializableType>(io::RuntimeSerializables::MESH);
}

bool MeshAsset::serialize(const io::Serializer* serializer, YAML::Emitter& emitter) {
	// Mesh
	emitter << YAML::Key << "Source" << YAML::Value << m_info.DataFile;

	// Layout
	emitter << YAML::Key << "IsStatic" << YAML::Value << m_info.IsStatic;
	return true;
}

bool MeshAsset::deserialize(const io::Serializer* serializer, const YAML::Node& data) {
	if (data["Source"]) {
		m_info.DataFile = data["Source"].as<io::LoadableHandle>();
	}

	m_info.IsStatic = data["IsStatic"].as<bool>();

	return true;
}

}
}
