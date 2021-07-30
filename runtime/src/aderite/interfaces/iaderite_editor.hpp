#pragma once

#include "aderite/core/scene/entity.hpp"

namespace aderite {
	namespace interfaces {

		/**
		 * @brief A special interface that is used to hookup a custom editor layer/object, functionality to
		 * aderite engine. By using this users can create a custom editor for example if a special lightweight 
		 * debug editor is needed, a user can implement the interface and attach it to the runtime.
		*/
		class iaderite_editor {
		public:
			virtual ~iaderite_editor() {}

			/**
			 * @brief Function invoked when the engine was initialized
			*/
			virtual void on_runtime_initialized() {}

			/**
			 * @brief Function invoked when the engine is about to shutdown
			*/
			virtual void on_runtime_shutdown() {}

			/**
			 * @brief Function invoked when the engine created a window and used to
			 * initialize a renderer
			*/
			virtual void on_renderer_initialized() {}

			/**
			 * @brief Function invoked when the engine is about to start rendering a frame
			*/
			virtual void on_start_render() {}

			/**
			 * @brief Function invoked when the engine is about to end rendering a frame
			*/
			virtual void on_end_render() {};

			/**
			 * @brief Function invoked when a new frame is about to be started
			*/
			virtual void on_begin_frame() {};

			/**
			 * @brief Function invoked when a frame has been ended
			*/
			virtual void on_end_frame() {};

			/**
			 * @brief Function invoked when a exit was requested from the engine
			*/
			virtual void on_requested_exit() {};

			/**
			 * @brief Function invoked when the currently selected entity is changed (could be changed through scripts)
			 * or invoked by a user in custom components
			 * @param entity The new active entity
			*/
			virtual void on_selected_entity_changed(scene::entity* entity) {};
		};

	}

	/**
	 * @brief Implementation of iaderite_editor, that does nothing, this is used as the default
	 * implementation.
	*/
	class null_editor : public interfaces::iaderite_editor {};
}
