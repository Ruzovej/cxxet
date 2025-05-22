#include "impl/event/list/list.hpp"

#include <doctest/doctest.h>

namespace rsm::impl {

TEST_CASE("event::list") {
  event::list l;
  long long n;

  SUBCASE("empty") {
    SUBCASE("default") {
      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("after reserve(1) = non-default capacity") {
      l.set_default_node_capacity(1);
      l.reserve();

      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    SUBCASE("prepend") {
      event::list l2;
      l.drain_and_prepend_other(l2);

      n = l.apply([](long long const, long long const, event::any const &) {
        REQUIRE(false);
      });
    }

    REQUIRE_EQ(n, 0);
    REQUIRE(l.empty());
    REQUIRE(l.size() == 0);
  }

  SUBCASE("append & apply") {
    int counter{0};

    constexpr int size{5};
    event::any a[size];

    new (&a[0].evt.begin) event::duration_begin{
        event::common{event::type::duration_begin, {}, "test begin"}, 0};

    new (&a[1].evt.end) event::duration_end{
        event::common{event::type::duration_end, {}, "test end"}, 5};

    new (&a[2].evt.cmp) event::complete{
        event::common{event::type::complete, {}, "test complete"}, 10, 15};

    new (&a[3].evt.cnt) event::counter{
        event::common{event::type::counter, {}, "test counter"}, 20, 42.666};

    new (&a[4].evt.inst) event::instant{
        event::common{event::type::instant, {}, "test instant"}, 25};

    SUBCASE("without reserve()") {
      l.append(a[0]);

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[0]);
        counter++;
      });

      REQUIRE_EQ(counter, 1);
    }

    SUBCASE("after reserve() with default capacity") {
      l.reserve();
      l.append(a[0]);
      l.append(a[1]);

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("after reserve(2) = non-default capacity") {
      l.set_default_node_capacity(2);
      l.reserve();
      for (auto const &evt : a) {
        l.append(evt);
      }

      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE_EQ(counter, size);
    }

    SUBCASE("prepend to empty") {
      event::list other;
      other.set_default_node_capacity(2);
      other.reserve();
      other.append(a[0]);
      other.append(a[1]);

      l.drain_and_prepend_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("prepend from empty") {
      event::list other;
      l.append(a[0]);
      l.append(a[1]);

      l.drain_and_prepend_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, 2);
    }

    SUBCASE("prepend") {
      event::list other;
      other.set_default_node_capacity(2);
      other.reserve();
      other.append(a[0]);
      other.append(a[1]);
      l.set_default_node_capacity(2);
      l.reserve();
      l.append(a[2]);
      l.append(a[3]);
      l.append(a[4]);

      l.drain_and_prepend_other(other);
      n = l.apply([&counter, &a](long long const, long long const,
                                 event::any const &evt) {
        REQUIRE_EQ(evt, a[counter++]);
      });

      REQUIRE(other.empty());
      REQUIRE_EQ(counter, size);
    }

    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(l.size(), n);
    REQUIRE(!l.empty());
  }
}

} // namespace rsm::impl
