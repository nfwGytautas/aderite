#pragma once

#include "aderite/utility/Macros.hpp"
#include "aderite/scene/Forward.hpp"

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
		 * @brief Function invoked when a exit was requested from the engine
		*/
		virtual void onRequestedExit() {};

		/**
		 * @brief Function invoked when a system update has happened
		*/
		virtual void onSystemUpdate(float delta) {};

		/**
		 * @brief Function invoked when a scene update has happened
		*/
		virtual void onSceneUpdate(float delta) {};

		/**
		 * @brief Function invoked when a physics update has happened
		*/
		virtual void onPhysicsUpdate(float delta) {};

		/**
		 * @brief Function invoked when a script update has happened
		*/
		virtual void onScriptUpdate(float delta) {};

		/**
		 * @brief Function invoked when a new scene was made active
		 * @param scene The scene that was made active
		*/
		virtual void onSceneChanged(scene::Scene* scene) {};
	};

/**
 * @brief Implementation of IEngineMiddleware, that does nothing, this is used as the default
 * implementation.
*/
class NullEditor : public interfaces::IEngineMiddleware {};

ADERITE_INTERFACE_NAMESPACE_END
