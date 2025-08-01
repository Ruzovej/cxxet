/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#ifdef CXXET_WITH_UNIT_TESTS

#include "impl/sink/cascade.hxx"
#include "impl/sink/event_collector.hxx"
#include "impl/sink/file_sink.hxx"

#include <thread>

#include <doctest/doctest.h>

namespace cxxet::impl {

template <typename base_sink> struct test_sink : base_sink {
  using base_sink::base_sink;

  template <typename callable_t> long long apply(callable_t &&callable) {
    const auto n{base_sink::events.apply(std::forward<callable_t>(callable))};
    return n;
  }

  [[nodiscard]] bool empty() const noexcept {
    return base_sink::events.empty();
  }
};

TEST_CASE("sink cascade") {
  long long n;
  int counter{0};

  constexpr int size{5};
  event::any a[size];

  new (&a[0].evt.dur_begin) event::duration_begin{1, 2, 3, "test begin", 0};

  new (&a[1].evt.dur_end) event::duration_end{4, 5, 6, "test end", 5};

  new (&a[2].evt.cmpl) event::complete{7, 8, 9, "test complete", 10, 15};

  new (&a[3].evt.cntr) event::counter{16, 17, 18, "test counter", 20, 42.666};

  new (&a[4].evt.inst) event::instant{
      19, 20, 21, "test instant", scope_t::global, 'c', 321, 1'111'111'111, 25};

  SUBCASE("one 'leaf'") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};
    test_sink<sink::event_collector> leaf{&root};
    leaf.reserve(2);

    leaf.append_event(a[0]);
    leaf.append_event(a[1]);

    n = root.apply([](long long const, long long const, event::any const &) {
      REQUIRE(false);
    });

    REQUIRE(root.empty());
    REQUIRE_EQ(n, 0);

    leaf.flush();

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("two 'leafs', tree") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};
    test_sink<sink::event_collector> leaf1{&root}, leaf2{&root};
    leaf1.reserve(1);
    leaf2.reserve(1);

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

  SUBCASE("two 'leafs', linear") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};
    test_sink<sink::event_collector> leaf1{&root}, leaf2{&leaf1};
    leaf1.reserve(1);
    leaf2.reserve(1);

    leaf1.append_event(a[0]);
    leaf2.append_event(a[1]);

    leaf2.flush();

    n = leaf1.apply([&counter, &a](long long const, long long const,
                                   event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(leaf2.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);

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
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};

    {
      sink::event_collector leaf{&root};
      leaf.reserve(traits.default_list_node_capacity);
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

  SUBCASE("intermediate sink::cascade<false> usage (thread UNsafe)") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};

    {
      sink::cascade<false> intermediate{&root};
      auto const test_fn = [&intermediate, &traits](event::any const &evt) {
        test_sink<sink::event_collector> leaf{&intermediate};
        leaf.reserve(traits.default_list_node_capacity);
        leaf.append_event(evt);
      };
      test_fn(a[0]);
      test_fn(a[1]);
    }

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      REQUIRE_EQ(evt, a[counter++]);
    });

    REQUIRE(!root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("intermediate sink::cascade<true> usage (thread_safe)") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<false>> root{traits};

    {
      sink::cascade<true> intermediate{&root};
      auto const test_fn = [&intermediate, &traits](event::any const &evt) {
        test_sink<sink::event_collector> leaf{&intermediate};
        leaf.reserve(traits.default_list_node_capacity);
        leaf.append_event(evt);
      };
      // same indexes ... because order wouldn't be guaranteed:
      std::thread t1{test_fn, a[0]};
      std::thread t2{test_fn, a[0]};

      test_fn(a[0]);

      t1.join();
      t2.join();
    }

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      ++counter;
      REQUIRE_EQ(evt, a[0]);
    });

    REQUIRE(!root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 3);
  }

  SUBCASE("sink::file_sink<true> usage (thread safe)") {
    sink::properties traits{};
    traits.set_target_filename("/dev/null");
    test_sink<sink::file_sink<true>> root{traits};

    auto const test_fn = [&root, &traits](event::any const &evt) {
      sink::event_collector leaf{&root};
      leaf.reserve(traits.default_list_node_capacity);
      leaf.append_event(evt);
    };

    std::thread t{test_fn, a[0]};
    test_fn(a[0]);
    t.join();

    n = root.apply([&counter, &a](long long const, long long const,
                                  event::any const &evt) {
      ++counter;
      REQUIRE_EQ(evt, a[0]);
    });

    REQUIRE(!root.empty());
    REQUIRE_EQ(n, counter);
    REQUIRE_EQ(counter, 2);
  }
}

} // namespace cxxet::impl

#endif
