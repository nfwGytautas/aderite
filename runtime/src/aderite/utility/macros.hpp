#pragma once

// ---------------------------------
// ADERITE MACROS
// ---------------------------------

#ifdef _DEBUG
#include <thread>
#include "aderite/aderite.hpp"

// Makes sure that the current thread is the same as the rendering thread
#define ASSERT_RENDER_THREAD assert(std::this_thread::get_id() == aderite::engine::get()->get_threader()->get_render_thread_id())

#endif
