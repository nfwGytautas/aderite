#pragma once

#include "aderite_editor/components/base_component.hpp"
#include "aderite_editor/components/modals.hpp"

namespace aderite {
	namespace editor {
		namespace components {

			/**
			 * @brief Toolbar of the editor, the bar that goes at the top of the window
			*/
			class toolbar : public base_component {
			public:
				toolbar();
				virtual ~toolbar();

				// Inherited via base_component
				virtual void render() override;
			private:
				modals::text_input* m_text_modal = nullptr;
			};

		}
	}
}