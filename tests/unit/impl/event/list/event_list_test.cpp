#include "impl/event/list/list.hpp"

#include <doctest/doctest.h>

namespace rsm::impl {

TEST_CASE("event::list") {
  event::list l;

  SUBCASE("empty") {
    SUBCASE("default") {
      auto const n{l.apply([](long long const, long long const,
                              event::any const &) { REQUIRE(false); })};

      REQUIRE_EQ(n, 0);
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      auto const n{l.apply([](long long const, long long const,
                              event::any const &) { REQUIRE(false); })};

      REQUIRE_EQ(n, 0);
    }

    SUBCASE("after reserve(1) = non-default capacity") {
      l.set_default_node_capacity(1);
      l.reserve();

      auto const n{l.apply([](long long const, long long const,
                              event::any const &) { REQUIRE(false); })};

      REQUIRE_EQ(n, 0);
    }
  }

  SUBCASE("append & apply") {
    constexpr int size{5};

    event::any a[size];

    new (&a[0].evt.begin) event::duration_begin{
        event::common{event::type::duration_begin, {}, "test begin"}, 5};

    new (&a[1].evt.end) event::duration_end{
        event::common{event::type::duration_end, {}, "test end"}, 7};

    new (&a[2].evt.cmp) event::complete{
        event::common{event::type::complete, {}, "test complete"}, 10, 20};

    new (&a[3].evt.cnt) event::counter{
        event::common{event::type::counter, {}, "test counter"}, 25, 42.666};

    new (&a[4].evt.inst) event::instant{
        event::common{event::type::instant, {}, "test instant"}, 30};

    SUBCASE("without reserve()") {
      l.append(a[0]);

      int counter{0};
      auto const n{l.apply([&counter, &a](long long const, long long const,
                                          event::any const &evt) {
        REQUIRE_EQ(evt, a[0]);
        counter++;
      })};

      REQUIRE_EQ(counter, 1);
      REQUIRE_EQ(n, counter);
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      l.append(a[0]);
      l.append(a[1]);

      int counter{0};
      auto const n{l.apply([&counter, &a](long long const, long long const,
                                          event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      })};

      REQUIRE_EQ(counter, 2);
      REQUIRE_EQ(n, counter);
    }

    SUBCASE("after reserve(2) = non-default capacity") {
      l.set_default_node_capacity(2);
      l.reserve();
      for (auto const &evt : a) {
        l.append(evt);
      }

      int counter{0};
      auto const n{l.apply([&counter, &a](long long const, long long const,
                                          event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      })};

      REQUIRE_EQ(counter, size);
      REQUIRE_EQ(n, counter);
    }
  }
}

} // namespace rsm::impl
