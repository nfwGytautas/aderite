#pragma once

#include "aderite/config.hpp"
#include "aderite/utility/macros.hpp"
#include "aderite/interfaces/IEngineMiddleware.hpp"

#include "aderite/asset/Forward.hpp"
#include "aderite/rendering/Forward.hpp"
#include "aderite/scene/Forward.hpp"
#include "aderite/window/Forward.hpp"

#define ADERITE_SYSTEM_PTR(public_name, class_name, field_name) public: static class_name* public_name() { return engine::get()->field_name; } private: class_name* field_name = nullptr;

ADERITE_ROOT_NAMESPACE_BEGIN

/**
 * @brief Main aderite engine instance
*/
class engine {
public:
	/**
	 * @brief Engine init options
	*/
	struct init_options {
		
	};

public:
	/**
	 * @brief Returns the instance of the engine
	*/
	static engine* get();

	/**
	 * @brief Initializes the aderite engine
	 * @param options Initialization options for the engine, some can be changed at runtime
	*/
	bool init(init_options options);

	/**
	 * @brief Shutdown aderite engine
	*/
	void shutdown();

	/**
	 * @brief Begins aderite engine loop
	*/
	void loop();

	/**
	 * @brief Request the engine to shutdown
	*/
	void request_exit();

	/**
	 * @brief Aborts a requested exit
	*/
	void abort_exit();

	// TODO: Rethink this
	void begin_frame();
	void end_frame();

	/**
	 * @brief Function is invoked when the Renderer was initialized
	*/
	void renderer_initialized();

	/**
	 * @brief Attaches a aderite editor instance to the runtime, previous one is deleted
	 * @param editor Editor to attach
	*/
	void attach_editor(interfaces::IEngineMiddleware* editor);
private:
	engine() {}
	engine(const engine& o) = delete;

private:
	bool m_wants_to_shutdown = false;
private:
	ADERITE_SYSTEM_PTR(get_window_manager, window::WindowManager, m_window_manager)
	ADERITE_SYSTEM_PTR(get_renderer, rendering::Renderer, m_renderer)
	ADERITE_SYSTEM_PTR(get_AssetManager, asset::AssetManager, m_AssetManager)
	ADERITE_SYSTEM_PTR(get_scene_manager, scene::SceneManager, m_scene_manager)

#if EDITOR_ENABLED == 1
	ADERITE_SYSTEM_PTR(get_editor, interfaces::IEngineMiddleware, m_editor)
#endif
};

ADERITE_ROOT_NAMESPACE_END
