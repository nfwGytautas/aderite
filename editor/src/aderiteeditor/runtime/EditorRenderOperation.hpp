#pragma once

#include <cstdint>
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
	virtual void execute() override;
	virtual void shutdown() override;

	ADERITE_DEBUG_SECTION
	(
		virtual const char* getOperationName() override { return "EditorRenderOperation"; }
	)
private:

};

ADERITE_EDITOR_RUNTIME_NAMESPACE_END
