#include "impl/sink.hpp"

#include <doctest/doctest.h>

namespace rsm::impl {

struct test_sink : sink {
  using sink::sink;

  template <typename callable_t> long long apply(callable_t &&callable) const {
    return events.apply(std::forward<callable_t>(callable));
  }
};

TEST_CASE("sink cascade") {
  long long n;
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

  SUBCASE("tree") {
    test_sink root{nullptr};
    test_sink leaf1{&root}, leaf2{&root};

    leaf1.append_event(a[0]);
    leaf2.append_event(a[1]);

    n = root.apply([](long long const, long long const, event::any const &) {
      REQUIRE(false);
    });

    REQUIRE_EQ(n, 0);

    leaf1.flush();

    n = root.apply([&a](long long const, long long const,
                        event::any const &evt) { REQUIRE_EQ(evt, a[0]); });

    REQUIRE(leaf1.empty());
    REQUIRE_EQ(n, 1);

    leaf2.flush();

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf2.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("linear") {
    test_sink root{nullptr};
    test_sink leaf1{&root}, leaf2{&leaf1};

    leaf1.append_event(a[0]);
    leaf2.append_event(a[1]);

    n = root.apply([](long long const, long long const, event::any const &) {
      REQUIRE(false);
    });

    REQUIRE(root.empty());
    REQUIRE_EQ(n, 0);

    leaf2.flush();

    n = leaf1.apply([&counter, &a](long long const, long long const,
                                   event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf2.empty());
    REQUIRE(root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(n, 2);

    leaf1.flush();
    counter = 0;

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf1.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("flush upon destruction") {
    test_sink root{nullptr};

    {
      test_sink leaf{&root};
      leaf.append_event(a[0]);
    }
    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(!root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 1);
  }
}

} // namespace rsm::impl
