#pragma once

#include "aderite/rendering/operation/Forward.hpp"
#include "aderite/rendering/operation/TargetProvideOperation.hpp"

#include "aderiteeditor/utility/Macros.hpp"

namespace aderite {
namespace editor {

/**
 * @brief Operation used to provide editor target
 */
class EditorTargetOperation : public rendering::TargetProvideOperation {
public:
    EditorTargetOperation();

    // Inherited from TargetProvideOperation
    virtual void initialize() override;
    virtual reflection::Type getType() const override;
};

} // namespace editor
} // namespace aderite
