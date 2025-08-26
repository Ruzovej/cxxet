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

#include <algorithm>
#include <array>
#include <thread>

#include <doctest/doctest.h>

#include "impl/sink/cascade.hxx"
#include "impl/sink/event_collector.hxx"
#include "impl/sink/file_sink.hxx"

namespace cxxet::impl {

namespace {

template <typename base_sink> struct test_sink : base_sink {
  using base_sink::base_sink;

  [[nodiscard]] bool empty() const noexcept {
    return base_sink::events.empty();
  }

  auto begin() const noexcept { return base_sink::events.cbegin(); }
  auto end() const noexcept { return base_sink::events.cend(); }
  auto cbegin() const noexcept { return base_sink::events.cbegin(); }
  auto cend() const noexcept { return base_sink::events.cend(); }
};

TEST_CASE("sink cascade") {
  std::size_t counter{0};

  constexpr int size{7};
  std::array<event::any, size> a;

  new (&a[0].evt.dur_begin)
      event::duration_begin{1, 2, output::category_flag{3}, "test begin", 0};

  new (&a[1].evt.dur_end)
      event::duration_end{4, 5, output::category_flag{6}, "test end", 5};

  new (&a[2].evt.cmpl)
      event::complete{7, 8, output::category_flag{9}, "test complete", 10, 15};

  new (&a[3].evt.cntr) event::counter{
      16, 17, output::category_flag{18}, "test counter", 20, 42.666};

  new (&a[4].evt.inst) event::instant{19,
                                      20,
                                      output::category_flag{21},
                                      "test instant",
                                      scope_t::global,
                                      'c',
                                      321,
                                      1'111'111'111,
                                      25};

  new (&a[5].evt.meta) event::metadata{'T',
                                       1234,
                                       output::category_flag{546},
                                       "some metadata str value ...",
                                       event::metadata_type::process_name,
                                       't',
                                       345,
                                       678978};

  new (&a[6].evt.meta) event::metadata{'N',
                                       1235,
                                       output::category_flag{7898797},
                                       42'000,
                                       event::metadata_type::thread_sort_index,
                                       'M',
                                       9330};

  SUBCASE("one 'leaf'") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};
    test_sink<sink::event_collector> leaf{&root};
    leaf.reserve(2);

    for (auto const &evt : a) {
      leaf.append_event(evt);
    }

    REQUIRE(!leaf.empty());
    REQUIRE(root.empty());
    leaf.flush();
    REQUIRE(leaf.empty());
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }

    REQUIRE_EQ(std::distance(leaf.cbegin(), leaf.cend()), 0);
    REQUIRE(leaf.empty());
    REQUIRE_EQ(counter, size);
  }

  SUBCASE("two 'leafs', tree") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};

    test_sink<sink::event_collector> leaf1{&root}, leaf2{&root};

    leaf1.reserve(1);
    leaf1.append_event(a[0]);

    leaf2.reserve(1);
    leaf2.append_event(a[1]);

    REQUIRE(!leaf1.empty());
    REQUIRE(root.empty());
    leaf1.flush();
    REQUIRE(leaf1.empty());
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(counter, 1);

    REQUIRE(!leaf2.empty());
    leaf2.flush();
    REQUIRE(leaf2.empty());

    counter = 0;
    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("two 'leafs', linear") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};

    test_sink<sink::event_collector> leaf1{&root}, leaf2{&leaf1};

    leaf1.reserve(1);
    leaf1.append_event(a[0]);

    leaf2.reserve(1);
    leaf2.append_event(a[1]);

    REQUIRE(!leaf2.empty());
    leaf2.flush();
    REQUIRE(leaf2.empty());
    REQUIRE(root.empty());

    for (auto const &p : leaf1) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(counter, 2);

    leaf1.flush();
    REQUIRE(leaf1.empty());
    REQUIRE(leaf2.empty());
    REQUIRE(!root.empty());

    counter = 0;
    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("flush upon destruction") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};

    {
      sink::event_collector leaf{&root};
      leaf.reserve(traits.default_list_node_capacity);
      leaf.append_event(a[0]);
      REQUIRE(root.empty());
    }
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 1);
  }

  SUBCASE("intermediate sink::cascade<false> usage (thread UNsafe)") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};

    {
      test_sink<sink::cascade<false>> intermediate{&root};
      auto const test_fn = [&intermediate, &traits](event::any const &evt) {
        test_sink<sink::event_collector> leaf{&intermediate};
        leaf.reserve(traits.default_list_node_capacity);
        leaf.append_event(evt);
      };
      REQUIRE(intermediate.empty());
      test_fn(a[0]);
      REQUIRE(!intermediate.empty());
      test_fn(a[1]);
      REQUIRE(root.empty());
    }
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      REQUIRE_EQ(p.event, a[counter++]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 2);
  }

  SUBCASE("intermediate sink::cascade<true> usage (thread_safe)") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<false>> root{traits};

    {
      test_sink<sink::cascade<true>> intermediate{&root};
      auto const test_fn = [&intermediate, &traits](event::any const &evt) {
        test_sink<sink::event_collector> leaf{&intermediate};
        leaf.reserve(traits.default_list_node_capacity);
        leaf.append_event(evt);
      };

      REQUIRE(intermediate.empty());
      { // same indexes ... because order wouldn't be guaranteed:
        std::thread t1{test_fn, a[0]};
        std::thread t2{test_fn, a[0]};
        test_fn(a[0]);
        t1.join();
        t2.join();
      }
      REQUIRE(!intermediate.empty());
      REQUIRE(root.empty());
    }
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      ++counter;
      REQUIRE_EQ(p.event, a[0]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 3);
  }

  SUBCASE("sink::file_sink<true> usage (thread safe)") {
    sink::properties traits{};
    traits.default_target_filename = "/dev/null";
    test_sink<sink::file_sink<true>> root{traits};

    auto const test_fn = [&root, &traits](event::any const &evt) {
      sink::event_collector leaf{&root};
      leaf.reserve(traits.default_list_node_capacity);
      leaf.append_event(evt);
    };

    REQUIRE(root.empty());
    {
      std::thread t{test_fn, a[0]};
      test_fn(a[0]);
      t.join();
    }
    REQUIRE(!root.empty());

    for (auto const &p : root) {
      ++counter;
      REQUIRE_EQ(p.event, a[0]);
    }
    REQUIRE_EQ(std::distance(root.cbegin(), root.cend()), counter);
    REQUIRE_EQ(counter, 2);
  }
}

} // namespace

} // namespace cxxet::impl

#endif
