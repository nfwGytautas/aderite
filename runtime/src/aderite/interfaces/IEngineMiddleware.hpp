#pragma once

#include "aderite/utility/Macros.hpp"

ADERITE_INTERFACE_NAMESPACE_BEGIN

	/**
	 * @brief A special interface that is used to hookup a custom editor layer/object, functionality to
	 * aderite engine. By using this users can create a custom editor for example if a special lightweight 
	 * debug editor is needed, a user can implement the interface and attach it to the runtime.
	*/
	class IEngineMiddleware {
	public:
		virtual ~IEngineMiddleware() {}

		/**
		 * @brief Function invoked when the engine was initialized
		*/
		virtual void onRuntimeInitialized() {}

		/**
		 * @brief Function invoked when the engine is about to shutdown
		*/
		virtual void onRuntimeShutdown() {}

		/**
		 * @brief Function invoked when the engine created a window and used it to
		 * initialize a Renderer
		*/
		virtual void onRendererInitialized() {}

		/**
		 * @brief Function invoked when the engine is about to start rendering a frame
		*/
		virtual void onStartRender() {}

		/**
		 * @brief Function invoked when the engine is about to end rendering a frame
		*/
		virtual void onEndRender() {};

		/**
		 * @brief Function invoked when a new frame is about to be started
		*/
		virtual void onBeginFrame() {};

		/**
		 * @brief Function invoked when a frame has been ended
		*/
		virtual void onEndFrame() {};

		/**
		 * @brief Function invoked when a exit was requested from the engine
		*/
		virtual void onRequestedExit() {};
	};

/**
 * @brief Implementation of IEngineMiddleware, that does nothing, this is used as the default
 * implementation.
*/
class NullEditor : public interfaces::IEngineMiddleware {};

ADERITE_INTERFACE_NAMESPACE_END
