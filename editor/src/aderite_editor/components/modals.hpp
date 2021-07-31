#pragma once

#include <functional>
#include <string>

#include "aderite_editor/components/base_component.hpp"

namespace aderite {
	namespace editor {
		namespace components {
			namespace modals {

				/**
				 * @brief Text input modal with two buttons
				*/
				class text_input : public base_component {
				public:
					using ConfirmFn = std::function<void(const std::string&)>;
					using CancelFn = std::function<void()>;

					/**
					 * @brief Input options
					*/
					struct Options {
						bool AllowEmpty = false;
					};

				public:
					text_input(const std::string& title = "Title", const std::string& text = "Text input modal", const Options& options = {});

					/**
					 * @brief Show modal
					*/
					void show();

					/**
					 * @brief Set the modal text
					*/
					void set_text(const std::string& text);

					/**
					 * @brief Set the modal title
					*/
					void set_title(const std::string& text);

					/**
					 * @brief Sets the confirm function to be invoked
					*/
					void set_confirm_action(ConfirmFn fn);

					/**
					 * @brief Sets the cancel function to be invoked
					*/
					void set_cancel_action(CancelFn fn);

					/**
					 * @brief Set the confirm button text
					*/
					void set_confirm_button_text(const std::string& text);

					// Inherited via base_component
					virtual void render() override;
				private:
					std::string m_text;
					std::string m_title;
					std::string m_confirm_text = "Confirm";
					std::string m_value = "";
					bool m_show = false;
					Options m_options = {};

					ConfirmFn m_confirm;
					CancelFn m_cancel;
				};

			}
		}
	}
}