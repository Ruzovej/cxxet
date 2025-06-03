#pragma once

#include "impl/event/any.hpp"

namespace cxxst::impl {

void thread_local_sink_submit_event(impl::event::any const &evt) noexcept;

}
