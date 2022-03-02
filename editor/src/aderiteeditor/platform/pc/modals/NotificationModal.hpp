#pragma once
#include <string>

#include "aderiteeditor/platform/pc/modals/IModal.hpp"

namespace aderite {
namespace editor {

/**
 * @brief A simple modal with 'Close' button used to just display a one shot message
 */
class NotificationModal : public IModal {
public:
    NotificationModal(const std::string& header, const std::string& message);

    // Inherited via IModal
    virtual void render() override;
    virtual bool stillValid() const override;
    virtual void close() override;
    virtual void show() override;

private:
    bool m_open = true;
    std::string m_header = "";
    std::string m_message = "";
};

} // namespace editor
} // namespace aderite
