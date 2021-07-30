#pragma once

#include "aderite/config.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/interfaces/iframe_object.hpp"
#include "aderite/interfaces/iaderite_editor.hpp"

// Forward declare?
#include "aderite/core/window/window_manager.hpp"
#include "aderite/core/rendering/renderer.hpp"
#include "aderite/core/threading/threader.hpp"
#include "aderite/core/assets/asset_manager.hpp"
#include "aderite/core/scene/scene_manager.hpp"

#define ADERITE_SYSTEM_PTR(public_name, class_name, field_name) public: static relay_ptr<class_name> public_name() { return engine::get()->field_name; } private: class_name* field_name = nullptr;

namespace aderite {

	/**
	 * @brief Main aderite engine instance
	*/
	class engine : public interfaces::iframe_object {
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

		// Inherited via iframe_object
		virtual void begin_frame() override;
		virtual void end_frame() override;

		/**
		 * @brief Function is invoked when the renderer was initialized
		*/
		void renderer_initialized();

		/**
		 * @brief Attaches a aderite editor instance to the runtime, previous one is deleted
		 * @param editor Editor to attach
		*/
		void attach_editor(interfaces::iaderite_editor* editor);
	private:
		engine() {}
		engine(const engine& o) = delete;

	private:
		bool m_wants_to_shutdown = false;
	private:
		ADERITE_SYSTEM_PTR(get_window_manager, window_manager, m_window_manager)
		ADERITE_SYSTEM_PTR(get_renderer, renderer, m_renderer)
		ADERITE_SYSTEM_PTR(get_threader, thread::threader, m_threader)
		ADERITE_SYSTEM_PTR(get_asset_manager, asset::asset_manager, m_asset_manager)
		ADERITE_SYSTEM_PTR(get_scene_manager, scene::scene_manager, m_scene_manager)

#if EDITOR_ENABLED == 1
		ADERITE_SYSTEM_PTR(get_editor, interfaces::iaderite_editor, m_editor)
#endif
	};

}
