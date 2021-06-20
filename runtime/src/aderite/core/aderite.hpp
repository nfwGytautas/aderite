#pragma once

#include "aderite/utility/pointer.hpp"
#include "aderite/interfaces/iframe_object.hpp"
#include "aderite/core/window/window_manager.hpp"
#include "aderite/core/rendering/renderer.hpp"
#include "aderite/core/threading/threader.hpp"

#define ADERITE_SYSTEM_PTR(public_name, class_name, field_name) public: relay_ptr<class_name> public_name() { return field_name.relay(); } private: ref<class_name> field_name;

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

		// Inherited via iframe_object
		virtual void begin_frame() override;
		virtual void end_frame() override;
	private:
		engine() {}
		engine(const engine& o) = delete;

	private:
		bool m_wants_to_shutdown = false;
		std::mutex m_mutex;
		std::condition_variable m_exit_cond;

	private:
		ADERITE_SYSTEM_PTR(get_window_manager, window_manager, m_window_manager)
		ADERITE_SYSTEM_PTR(get_renderer, renderer, m_renderer)
		ADERITE_SYSTEM_PTR(get_threader, thread::threader, m_threader)
	};

}
