#include "layer.hpp"

#include "aderite/utility/log.hpp"
#include "aderite/utility/pointer.hpp"
#include "aderite/core/threading/thread_invoke.hpp"
#include "aderite/core/aderite.hpp"

namespace aderite {
	void layer::i_init() {
		// Initialize rest of the systems on rendering thread
		ref<thread::void_invoke> invoke = new thread::void_invoke([&]() {
			this->init();
		});

		engine::get()->get_threader()->get_render_thread()->invoke(invoke.as<thread::thread_invoke_base>());
	}

	void layer::i_shutdown() {
		// Initialize rest of the systems on rendering thread
		ref<thread::void_invoke> invoke = new thread::void_invoke([&]() {
			this->shutdown();
		});

		engine::get()->get_threader()->get_render_thread()->invoke(invoke.as<thread::thread_invoke_base>());
	}

	layer::layer()
	{}

	layer::~layer()
	{}

}
