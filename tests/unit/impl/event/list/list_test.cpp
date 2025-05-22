#include "impl/event/list/list.hpp"

#include <doctest/doctest.h>

namespace rsm::impl::event {

TEST_CASE("empty") {
  list l;

  SUBCASE("default") {
    l.apply(
        [](long long const, long long const, any const &) { REQUIRE(false); });
  }

  SUBCASE("after reserve() with default capacity") {
    l.reserve();
    l.apply(
        [](long long const, long long const, any const &) { REQUIRE(false); });
  }

  SUBCASE("after reserve(1) = non-default capacity") {
    l.set_default_node_capacity(1);
    l.reserve();
    l.apply(
        [](long long const, long long const, any const &) { REQUIRE(false); });
  }
}

TEST_CASE("append & apply") {
  list l;

  constexpr int size{5};

  any a[size];

  new (&a[0].evt.begin)
      duration_begin{common{type::duration_begin, {}, "test begin"}, 5};
  new (&a[1].evt.end)
      duration_end{common{type::duration_end, {}, "test end"}, 7};
  new (&a[2].evt.cmp)
      complete{common{type::complete, {}, "test complete"}, 10, 20};
  new (&a[3].evt.cnt)
      counter{common{type::counter, {}, "test counter"}, 25, 42.666};
  new (&a[4].evt.inst) instant{common{type::instant, {}, "test instant"}, 30};

  SUBCASE("without reserve()") {
    l.append(a[0]);

    int counter{0};
    l.apply([&counter, &a](long long const, long long const, any const &evt) {
      REQUIRE_EQ(evt, a[0]);
      counter++;
    });

    REQUIRE_EQ(counter, 1);
  }

  SUBCASE("after reserve() with default capacity") {
    l.reserve();
    l.append(a[0]);
    l.append(a[1]);

    int counter{0};
    l.apply([&counter, &a](long long const, long long const, any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("after reserve(2) = non-default capacity") {
    l.set_default_node_capacity(2);
    l.reserve();
    for (auto &evt : a) {
      l.append(evt);
    }

    int counter{0};
    l.apply([&counter, &a](long long const, long long const, any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE_EQ(counter, size);
  }
}

} // namespace rsm::impl::event
