#pragma once

#include "impl/records.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

void dump_records(records const *first, long long const time_point_zero, output::format const fmt,
                               char const *const filename);

}
