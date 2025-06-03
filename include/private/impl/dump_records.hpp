#pragma once

#include "cxxst/output_format.hpp"
#include "impl/event/list/list.hpp"

namespace cxxst::impl {

void dump_records(impl::event::list const &list,
                  long long const time_point_zero, output::format const fmt,
                  char const *const filename);

}
