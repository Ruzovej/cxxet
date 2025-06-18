#pragma once

#include "impl/event/any.hxx"

namespace cxxst::impl {

void thread_local_sink_submit_event(impl::event::any const &evt) noexcept;

}
