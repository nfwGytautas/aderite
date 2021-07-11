#include "layer.hpp"

#include "aderite/aderite.hpp"
#include "aderite/utility/macros.hpp"

namespace aderite {
	void layer::i_init() {
		ASSERT_RENDER_THREAD;
		this->init();
	}

	void layer::i_shutdown() {
		ASSERT_RENDER_THREAD;
		this->shutdown();
	}

	layer::layer()
		: renderer(engine::get()->get_renderer()), assets(engine::get()->get_asset_manager())
	{}

	layer::~layer()
	{}

}
