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

#pragma once

#include <cassert>

#include "impl/event/common.hxx"
#include "impl/event/metadata_type.hxx"

namespace cxxet::impl::event {

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.lenwiilchoxp

struct metadata {
  static constexpr type_t t{type_t::metadata};

  common<t> evt;
  metadata_type type;
  char i_flag_1;  // explicit padding - unspecified meaning
  short i_flag_2; // explicit padding - unspecified meaning
  int i_flag_4; // explicit padding - "sometimes" used for integer value ("sort
                // index")

  metadata() = default;
  constexpr metadata(char const *const aDesc,
                     metadata_type const aType) noexcept
      : evt{aDesc}, type{aType}, i_flag_1{0}, i_flag_2{0}, i_flag_4{0} {
    assert(type == metadata_type::process_name ||
           type == metadata_type::process_labels ||
           type == metadata_type::thread_name);
  }
  constexpr metadata(int const sort_index, metadata_type const aType) noexcept
      : evt{nullptr}, type{aType}, i_flag_1{0}, i_flag_2{0},
        i_flag_4{sort_index} {
    assert(type == metadata_type::process_sort_index ||
           type == metadata_type::thread_sort_index);
  }

  [[nodiscard]] constexpr metadata_type get_metadata_type() const noexcept {
    return type;
  }

  [[nodiscard]] char const *get_name() const {
#define CXXET_IMPL_METADATA_CASE(kind)                                         \
  case metadata_type::kind:                                                    \
    return #kind

    switch (type) {
      CXXET_IMPL_METADATA_CASE(process_name);
      CXXET_IMPL_METADATA_CASE(process_labels);
      CXXET_IMPL_METADATA_CASE(process_sort_index);
      CXXET_IMPL_METADATA_CASE(thread_name);
      CXXET_IMPL_METADATA_CASE(thread_sort_index);
    default:
      throw "Unknown metadata_type";
    }

#undef CXXET_IMPL_METADATA_CASE
  }

  [[nodiscard]] char const *get_arg_name() const {
    switch (type) {
    case metadata_type::process_labels:
      return "labels";
    case metadata_type::process_name:
    case metadata_type::thread_name:
      return "name";
    case metadata_type::process_sort_index:
    case metadata_type::thread_sort_index:
      return "sort_index";
    default:
      throw "Unknown metadata_type";
    }
  }

  [[nodiscard]] constexpr char const *get_arg_value_str() const noexcept {
    assert(type == metadata_type::process_name ||
           type == metadata_type::process_labels ||
           type == metadata_type::thread_name);
    return evt.desc;
  }

  [[nodiscard]] constexpr int get_arg_value_int() const noexcept {
    assert(type == metadata_type::process_sort_index ||
           type == metadata_type::thread_sort_index);
    return i_flag_4;
  }

  [[nodiscard]] constexpr bool
  operator==(metadata const &other) const noexcept {
    auto const tie = [](metadata const &i) {
      return std::tie(i.evt, i.type, i.i_flag_1, i.i_flag_2, i.i_flag_4);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace cxxet::impl::event
