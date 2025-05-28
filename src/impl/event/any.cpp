#include "impl/event/any.hpp"

#include <cstddef>

#include <algorithm>
#include <type_traits>
#include <variant>

namespace rsm::impl::event {

namespace {

// Are those meaningful tests?!

struct common_without_explicit_padding {
  type_t const t;
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

static constexpr std::size_t max_size{
    std::max({sizeof(duration_begin), sizeof(duration_end), sizeof(complete),
              sizeof(instant), sizeof(counter)})};

static constexpr std::size_t min_size{
    std::min({sizeof(duration_begin), sizeof(duration_end), sizeof(complete),
              sizeof(instant), sizeof(counter)})};

static_assert(min_size < max_size); // TODO add explicit padding to every event
// and change this to equality

static_assert(sizeof(any) == max_size,
              "`any` should have same size as the internal union it wraps!");

} // namespace rsm::impl::event
