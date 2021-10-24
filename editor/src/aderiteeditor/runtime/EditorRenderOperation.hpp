#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <bgfx/bgfx.h>
#include "aderite/physics/Forward.hpp"
#include "aderite/physics/collider/Forward.hpp"
#include "aderite/scene/components/Forward.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"
#include "aderiteeditor/utility/Macros.hpp"

ADERITE_EDITOR_RUNTIME_NAMESPACE_BEGIN

/**
 * @brief Operation used to provide a debug rendering (colliders, gizmos, etc.)
*/
class EditorRenderOperation : public rendering::OperationBase {
	// Constants
	static constexpr uint16_t c_ViewId = 200;
public:
	EditorRenderOperation();
	~EditorRenderOperation();

	// Inherited via OperationBase
	virtual void initialize() override;
	virtual void execute(rendering::PipelineState* state) override;
	virtual void shutdown() override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
private:
	void updateUniform();

	// Rendering operations
	void renderPhysicsObjects();
	void renderBoxCollider(physics::BoxCollider* collider, const scene::components::TransformComponent& transform);

	// Helpers
	void loadMeshes();
	void loadShaders();
	void loadShader(const std::filesystem::path& base, bgfx::ProgramHandle& shader, const std::string& name);
private:
	bgfx::ProgramHandle m_wireframeShader = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle m_editorUniform = BGFX_INVALID_HANDLE;

	bgfx::VertexBufferHandle m_bcCube = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle m_bcCubeIbo = BGFX_INVALID_HANDLE;

	// Uniform data
	union
	{
		struct
		{
			/*0*/ struct { float wfColor[3]; float wfOpacity; };
			/*1*/ struct { float wfThickness, unused[3]; };
		};

		float EditorParameters[2 * 4];
	};
};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END