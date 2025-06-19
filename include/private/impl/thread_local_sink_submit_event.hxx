#pragma once

#include "impl/event/any.hxx"

namespace cxxet::impl {

void thread_local_sink_submit_event(impl::event::any const &evt) noexcept;

}
