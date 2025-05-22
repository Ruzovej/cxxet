#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"

#include <doctest/doctest.h>

namespace rsm::impl {

template <typename base_sink> struct test_sink : base_sink {
  using base_sink::base_sink;

  template <typename callable_t> long long apply(callable_t &&callable) {
    const auto n{base_sink::events.apply(std::forward<callable_t>(callable))};
    // base_sink::events.destroy();
    return n;
  }
};

TEST_CASE("sink cascade") {
  long long n;
  int counter{0};

  constexpr int size{5};
  event::any a[size];

  new (&a[0].evt.begin) event::duration_begin{
      event::common{event::type::duration_begin, 1, 2, 3, "test begin"}, 0};

  new (&a[1].evt.end) event::duration_end{
      event::common{event::type::duration_end, 4, 5, 6, "test end"}, 5};

  new (&a[2].evt.cmp) event::complete{
      event::common{event::type::complete, 7, 8, 9, "test complete"}, 10, 15};

  new (&a[3].evt.cnt) event::counter{
      event::common{event::type::counter, 16, 17, 18, "test counter"}, 20,
      42.666};

  new (&a[4].evt.inst) event::instant{
      event::common{event::type::instant, 19, 20, 21, "test instant"}, 25};

  SUBCASE("tree") {
    test_sink<central_sink> root{};
    root.set_target_filename("/dev/null");
    test_sink<local_sink> leaf1{&root}, leaf2{&root};

    leaf1.append_event(a[0]);
    leaf2.append_event(a[1]);

    n = root.apply([](long long const, long long const, event::any const &) {
      REQUIRE(false);
    });

    REQUIRE_EQ(n, 0);

    leaf1.flush_to_parent();

    n = root.apply([&a](long long const, long long const,
                        event::any const &evt) { REQUIRE_EQ(evt, a[0]); });

    REQUIRE(leaf1.empty());
    REQUIRE_EQ(n, 1);

    leaf2.flush_to_parent();

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf2.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("linear") {
    test_sink<central_sink> root{};
    root.set_target_filename("/dev/null");
    test_sink<local_sink> leaf1{&root}, leaf2{&leaf1};

    leaf1.append_event(a[0]);
    leaf2.append_event(a[1]);

    n = root.apply([](long long const, long long const, event::any const &) {
      REQUIRE(false);
    });

    REQUIRE(root.empty());
    REQUIRE_EQ(n, 0);

    leaf2.flush_to_parent();

    n = leaf1.apply([&counter, &a](long long const, long long const,
                                   event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf2.empty());
    REQUIRE(root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(n, 2);

    leaf1.flush_to_parent();
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
    test_sink<central_sink> root{};
    root.set_target_filename("/dev/null");

    {
      test_sink<local_sink> leaf{&root};
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
