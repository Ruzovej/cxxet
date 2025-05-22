#include "impl/event/any.hpp"

#include <type_traits>

namespace rsm::impl::event {

// how to automatically keep this list up to date?! Or maybe put it to separate
// & corresponding files?
static_assert(std::is_trivially_destructible_v<duration_begin>);
static_assert(std::is_trivially_destructible_v<duration_end>);
static_assert(std::is_trivially_destructible_v<complete>);
static_assert(std::is_trivially_destructible_v<instant>);
static_assert(std::is_trivially_destructible_v<counter>);

static_assert(std::is_trivially_destructible_v<any>);

} // namespace rsm::impl::event
