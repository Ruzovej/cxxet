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

#include "impl/write_out/category_name_map.hxx"

#include <cctype>

#include <algorithm>
#include <stdexcept>

namespace cxxet::impl::write_out {

namespace {

constexpr bool is_one_bit_set(unsigned const x) noexcept {
  return x && !(x & (x - 1));
}

constexpr unsigned bit_to_index(unsigned const x) noexcept {
  unsigned idx{0};
  unsigned val{1};

  while (val != x) {
    val <<= 1;
    ++idx;
  }

  return idx;
}

} // namespace

category_name_map::category_name_map() noexcept = default;

unsigned category_name_map::register_category_name(unsigned const category,
                                                   std::string &&name,
                                                   bool const allow_rename) {
  if (!is_name_valid(name)) {
    throw std::runtime_error("category name is not valid");
  }

  if (is_name_used(name)) {
    throw std::runtime_error("category name already used");
  }

  if (category == 0) {
    for (unsigned idx = 0; idx < max_user_categories; ++idx) {
      bool const past_the_end{idx >= names.size()};

      if (past_the_end || names[idx].empty()) {
        if (past_the_end) {
          names.resize(idx + 1);
        }
        names[idx] = std::move(name);

        return 1u << idx;
      }
    }

    throw std::runtime_error("no free category flags available");
  }

  if (!is_one_bit_set(category)) {
    throw std::runtime_error("category flag must have exactly one bit set");
  }

  auto const idx{bit_to_index(category)};

  if (idx >= max_user_categories) {
    throw std::runtime_error("category flag trying to use reserved high bit");
  }

  if (names.size() <= idx) {
    names.resize(idx + 1);
  }

  if (!names[idx].empty() && !allow_rename) {
    throw std::runtime_error("category flag already registered");
  }

  names[idx] = std::move(name);

  return category;
}

bool category_name_map::is_name_valid(std::string_view const name) noexcept {
  return !name.empty() && (std::isalpha(static_cast<int>(name[0])) != 0) &&
         std::all_of(name.cbegin() + 1, name.cend(), [](char const c) {
           return (std::isalnum(static_cast<int>(c)) != 0) || (c == '-') ||
                  (c == '_');
         });
}

bool category_name_map::is_name_used(
    std::string_view const name) const noexcept {
  return std::count_if(names.cbegin(), names.cend(),
                       [name](auto const &n) { return n == name; }) > 0;
}

std::string category_name_map::get_joined_category_names(
    unsigned const category_bits) const {
  std::string res;

  for (unsigned idx{0u}; idx < num_categories; ++idx) {
    auto const mask{1u << idx};
    if ((category_bits & mask) == 0u) {
      continue;
    }

    if ((mask == reserved_1) || (mask == reserved_2)) {
      if (!res.empty()) {
        res += ',';
      }
      res.append((mask == reserved_1) ? reserved_1_name.data()
                                      : reserved_2_name.data(),
                 (mask == reserved_1) ? reserved_1_name.size()
                                      : reserved_2_name.size());
      continue;
    }

    if (idx >= names.size()) {
      continue;
    }

    if (!names[idx].empty()) {
      if (!res.empty()) {
        res += ',';
      }
      res += names[idx];
    } // else "unknown" category - just skip it?!
  }

  return res;
}

} // namespace cxxet::impl::write_out

#ifdef CXXET_WITH_UNIT_TESTS

#include <doctest/doctest.h>

namespace cxxet::impl::write_out {

namespace {

TEST_CASE("write_out::category_name_map helpers") {
  SUBCASE("is_one_bit_set") {
    SUBCASE("all valid cases") {
      for (unsigned i{0u}; i < 32u; ++i) {
        REQUIRE_EQ(is_one_bit_set(1u << i), true);
      }
    }

    SUBCASE("some invalid cases") {
      REQUIRE_EQ(is_one_bit_set(0u), false);
      REQUIRE_EQ(is_one_bit_set(3u), false);
      REQUIRE_EQ(is_one_bit_set(5u), false);
      REQUIRE_EQ(is_one_bit_set(6u), false);
      REQUIRE_EQ(is_one_bit_set(7u), false);
      REQUIRE_EQ(is_one_bit_set(1u | 2u), false);
      REQUIRE_EQ(is_one_bit_set(128u | 4069u), false);
      REQUIRE_EQ(is_one_bit_set(0xFF'FF'FF'FFu), false);
    }
  }

  SUBCASE("bit_to_index") {
    for (unsigned i{0u}; i < 32u; ++i) {
      REQUIRE_EQ(bit_to_index(1u << i), i);
    }
  }

  SUBCASE("category_name_map::is_name_valid") {
    SUBCASE("valid names") {
      REQUIRE_EQ(category_name_map::is_name_valid("valid_name"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("valid-name"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("validName123"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("v"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("v-"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("v-_"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("v-_-----------_____"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("V"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("name_with_underscores"),
                 true);
      REQUIRE_EQ(category_name_map::is_name_valid("name-with-dashes"), true);
      REQUIRE_EQ(category_name_map::is_name_valid("nameWithMixed-Chars_123"),
                 true);
    }

    SUBCASE("invalid names") {
      REQUIRE_EQ(category_name_map::is_name_valid(""), false);
      REQUIRE_EQ(category_name_map::is_name_valid("1"), false);
      REQUIRE_EQ(category_name_map::is_name_valid("-"), false);
      REQUIRE_EQ(category_name_map::is_name_valid("_"), false);
      REQUIRE_EQ(category_name_map::is_name_valid("_some_text"), false);
      REQUIRE_EQ(category_name_map::is_name_valid("-sometext"), false);
      REQUIRE_EQ(category_name_map::is_name_valid("123sometext"), false);
      REQUIRE_EQ(
          category_name_map::is_name_valid("invalid name because of spaces"),
          false);
      REQUIRE_EQ(category_name_map::is_name_valid(
                     "invalid-name-because-of-exclamation-mark!"),
                 false);
    }
  }
}

TEST_CASE("write_out::category_name_map itself") {
  category_name_map cnm;

  SUBCASE("optimistic cases") {
    SUBCASE("no registration") {
      REQUIRE_EQ(cnm.get_joined_category_names(0), "");
      REQUIRE_EQ(cnm.get_joined_category_names(1u << 30), "cxxet_1");
      REQUIRE_EQ(cnm.get_joined_category_names(1u << 31), "cxxet_2");
      REQUIRE_EQ(cnm.get_joined_category_names(1u << 30 | 1u << 31),
                 "cxxet_1,cxxet_2");
    }

    SUBCASE("single registration") {
      auto const category{
          cnm.register_category_name(0, "test_category", false)};

      REQUIRE_EQ(category, 1u);

      REQUIRE_EQ(cnm.get_joined_category_names(category), "test_category");
    }

    SUBCASE("multiple registrations") {
      auto const cat1{cnm.register_category_name(0, "c1", false)};
      auto const cat2{cnm.register_category_name(0, "c2", false)};
      auto const cat3{cnm.register_category_name(1u << 15, "c3", false)};

      REQUIRE_EQ(cat1, 1u);
      REQUIRE_EQ(cat2, 2u);
      REQUIRE_EQ(cat3, 1u << 15);

      REQUIRE_EQ(cnm.get_joined_category_names(0), "");
      REQUIRE_EQ(cnm.get_joined_category_names(cat1), "c1");
      REQUIRE_EQ(cnm.get_joined_category_names(cat1 | cat1), "c1");
      REQUIRE_EQ(cnm.get_joined_category_names(cat1 & cat1), "c1");
      REQUIRE_EQ(cnm.get_joined_category_names(cat1 | cat2), "c1,c2");
      REQUIRE_EQ(cnm.get_joined_category_names(cat1 | cat3), "c1,c3");
      REQUIRE_EQ(cnm.get_joined_category_names(cat2 | cat1 | cat3), "c1,c2,c3");
    }

    SUBCASE("auto-assign exhaustion") {
      for (unsigned i{0u}; i < category_name_map::max_user_categories; ++i) {
        auto const cat{
            cnm.register_category_name(0, "Cat-" + std::to_string(i), false)};
        REQUIRE_EQ(cat, 1u << i);
      }

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(0, "one_too_many", false),
          std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(
                            0, "one_too_many_regardless_of_rename", true),
                        std::runtime_error);

      REQUIRE_EQ(cnm.get_joined_category_names(0), "");
      REQUIRE_EQ(
          cnm.get_joined_category_names(0x3F'FF'FF'FFu),
          R"long_string(Cat-0,Cat-1,Cat-2,Cat-3,Cat-4,Cat-5,Cat-6,Cat-7,Cat-8,Cat-9,Cat-10,Cat-11,Cat-12,Cat-13,Cat-14,Cat-15,Cat-16,Cat-17,Cat-18,Cat-19,Cat-20,Cat-21,Cat-22,Cat-23,Cat-24,Cat-25,Cat-26,Cat-27,Cat-28,Cat-29)long_string");
      REQUIRE_EQ(
          cnm.get_joined_category_names(0xFF'FF'FF'FFu),
          R"long_string(Cat-0,Cat-1,Cat-2,Cat-3,Cat-4,Cat-5,Cat-6,Cat-7,Cat-8,Cat-9,Cat-10,Cat-11,Cat-12,Cat-13,Cat-14,Cat-15,Cat-16,Cat-17,Cat-18,Cat-19,Cat-20,Cat-21,Cat-22,Cat-23,Cat-24,Cat-25,Cat-26,Cat-27,Cat-28,Cat-29,cxxet_1,cxxet_2)long_string");
    }

    SUBCASE("valid single bit") {
      auto const category{
          cnm.register_category_name(8u, "test_category", false)};
      REQUIRE_EQ(category, 8u);

      REQUIRE_EQ(cnm.get_joined_category_names(4u), "");
      REQUIRE_EQ(cnm.get_joined_category_names(16u), "");
      REQUIRE_EQ(cnm.get_joined_category_names(4u | 16u), "");

      REQUIRE_EQ(cnm.get_joined_category_names(4u | 8u | 16u), "test_category");
      REQUIRE_EQ(cnm.get_joined_category_names(0x3F'FF'FF'FFu),
                 "test_category");
      REQUIRE_EQ(cnm.get_joined_category_names(0xFF'FF'FF'FFu),
                 "test_category,cxxet_1,cxxet_2");
    }

    SUBCASE("valid single bit - different positions") {
      auto const cat1{cnm.register_category_name(1u, "cat1", false)};
      auto const cat2{cnm.register_category_name(4u, "cat2", false)};
      auto const cat3{cnm.register_category_name(16u, "cat3", false)};

      REQUIRE_EQ(cat1, 1u);
      REQUIRE_EQ(cat2, 4u);
      REQUIRE_EQ(cat3, 16u);
    }

    SUBCASE("allow rename") {
      auto const cat1{cnm.register_category_name(2u, "original_name", false)};
      REQUIRE_EQ(cat1, 2u);
      REQUIRE_EQ(cnm.get_joined_category_names(cat1), "original_name");

      auto const cat2{cnm.register_category_name(2u, "new_name", true)};
      REQUIRE_EQ(cat2, 2u);
      REQUIRE_EQ(cnm.get_joined_category_names(0x3F'FF'FF'FF), "new_name");
    }
  }

  SUBCASE("error cases") {
    SUBCASE("invalid names") {
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, "", false),
                        std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, " invalid", false),
                        std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, "1invalid", false),
                        std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, "-invalid", false),
                        std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, "_invalid", false),
                        std::runtime_error);
      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(0u, "invalid name", false),
          std::runtime_error);
      REQUIRE_THROWS_AS((void)cnm.register_category_name(0u, "invalid!", false),
                        std::runtime_error);
    }

    SUBCASE("used name") {
      (void)cnm.register_category_name(0u, "some_name_1", false);

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(0u, "some_name_1", false),
          std::runtime_error);

      (void)cnm.register_category_name(0u, "some_name_2", false);
      (void)cnm.register_category_name(0u, "some_name_3", false);
      auto const cat4{cnm.register_category_name(0u, "some_name_4", false)};

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(0u, "some_name_2", false),
          std::runtime_error);
      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(0u, "some_name_3", true),
          std::runtime_error);
      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(cat4, "some_name_4", true),
          std::runtime_error); // regardless of the position (same in this case)
    }

    SUBCASE("category with multiple bits set") {
      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(1u | 2u, "invalid", false),
          std::runtime_error);

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(2u | 4u, "invalid", false),
          std::runtime_error);

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(1u | 2u | 4u, "invalid", false),
          std::runtime_error);
    }

    SUBCASE("category using reserved bits") {
      // Test highest 2 bits which should be reserved
      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(
              1u << category_name_map::max_user_categories, "reserved1", false),
          std::runtime_error);

      REQUIRE_THROWS_AS((void)cnm.register_category_name(
                            1u << (category_name_map::max_user_categories + 1),
                            "reserved2", false),
                        std::runtime_error);
    }

    SUBCASE("duplicate registration without allow_rename") {
      auto const cat1{cnm.register_category_name(4u, "original", false)};
      REQUIRE_EQ(cat1, 4u);

      REQUIRE_THROWS_AS(
          (void)cnm.register_category_name(4u, "duplicate", false),
          std::runtime_error);
    }
  }
}

} // namespace

} // namespace cxxet::impl::write_out

#endif
