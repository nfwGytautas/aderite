#pragma once

#include <unordered_map>
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/rendering/PipelineState.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to create an entity provider
*/
class EntityProvideOperation : public OperationBase {
public:
	// Inherited via OperationBase
	virtual void execute(PipelineState* state) override;
	virtual reflection::Type getType() const override;
	virtual bool serialize(const io::Serializer* serializer, YAML::Emitter& emitter) override;
	virtual bool deserialize(io::Serializer* serializer, const YAML::Node& data) override;
public:
	/**
	 * @brief Validates entity
	 * @param meta Meta component
	 * @param mrenderer Mesh renderer component
	 * @return True if valid and can be rendered, false otherwise
	*/
	bool validateEntity(
		scene::components::MetaComponent& meta,
		scene::components::MeshRendererComponent& mrenderer);

	/**
	 * @brief Cleans the drawcall list (removes transformations)
	*/
	void cleanList();
private:
	std::unordered_map<size_t, size_t> m_lookup;
	DrawCallList m_drawcalls;
};

ADERITE_RENDERING_NAMESPACE_END
