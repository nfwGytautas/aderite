#pragma once

#include <unordered_map>
#include "aderite/rendering/DrawCall.hpp"
#include "aderite/rendering/operation/OperationBase.hpp"
#include "aderite/rendering/operation/Forward.hpp"

ADERITE_RENDERING_NAMESPACE_BEGIN

/**
 * @brief Operation used to create an entity provider
*/
class EntityProvideOperation : public OperationBase {
public:
	using DrawCallList = std::vector<DrawCall>;

	// TODO: Check if this can be a const return
	/**
	 * @brief Returns the draw call list
	 * NOTE: The return is a reference which allows other operations to edit the draw call list, however
	 *       these operations should make sure that the vector itself isn't modified
	 */
	DrawCallList& getDrawCalls();

	// Inherited via OperationBase
	virtual void execute() override;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "EntityProvideOperation"; }
	)
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
	std::unordered_map<size_t, DrawCallList::iterator> m_lookup;
	DrawCallList m_drawcalls;
};

ADERITE_RENDERING_NAMESPACE_END
