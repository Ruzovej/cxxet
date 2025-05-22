#include "impl/event/any.hpp"

#include <type_traits>
#include <variant>

namespace rsm::impl::event {

namespace {

// Are those meaningful tests?!

struct common_without_explicit_padding {
  type const t;
  char const *const desc;
};

static_assert(sizeof(common<>) <= sizeof(common_without_explicit_padding));

struct common_without_type {
  char const *const desc;
  int const flag_4;
  short const flag_2;
  char const flag_1;
}; // even shuffling members doesn't help ...:

static_assert(sizeof(common<>) < sizeof(std::variant<common_without_type>));

} // namespace

// how to automatically keep this list up to date?! Or maybe put it to separate
// & corresponding files?
static_assert(std::is_trivially_destructible_v<duration_begin>);
static_assert(std::is_trivially_destructible_v<duration_end>);
static_assert(std::is_trivially_destructible_v<complete>);
static_assert(std::is_trivially_destructible_v<instant>);
static_assert(std::is_trivially_destructible_v<counter>);

static_assert(std::is_trivially_destructible_v<any>);

} // namespace rsm::impl::event
