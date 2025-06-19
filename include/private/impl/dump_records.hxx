#pragma once

#include "cxxet/output_format.hxx"
#include "impl/event/list/list.hxx"

namespace cxxet::impl {

void dump_records(impl::event::list const &list,
                  long long const time_point_zero, output::format const fmt,
                  char const *const filename);

}
