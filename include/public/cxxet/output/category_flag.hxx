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

#include <cstddef>

namespace cxxet::output {

namespace detail {
enum category_flag_none_e : unsigned { none = 0 };
}

struct category_flag {
  constexpr explicit category_flag(
      detail::category_flag_none_e const =
          detail::category_flag_none_e::none) noexcept
      : value(0) {}

  constexpr explicit category_flag(unsigned const aValue) : value(aValue) {}

  constexpr bool operator&(const category_flag &other) const noexcept {
    return (value & other.value) != 0;
  }

  constexpr category_flag operator|(const category_flag &other) const noexcept {
    return category_flag{value | other.value};
  }

  constexpr category_flag &operator|=(const category_flag &other) noexcept {
    value |= other.value;
    return *this;
  }

  constexpr bool is_one_bit_set() const noexcept {
    return value && !(value & (value - 1));
  }

  constexpr unsigned bit_to_index() const noexcept {
    unsigned idx{0};
    unsigned val{1};

    while (val != value) {
      val <<= 1;
      ++idx;
    }

    return idx;
  }

  constexpr bool operator==(const category_flag &other) const noexcept {
    return value == other.value;
  }

  constexpr bool operator!=(const category_flag &other) const noexcept {
    return value != other.value;
  }

  struct hasher {
    constexpr std::size_t operator()(const category_flag &flag) const noexcept {
      // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
      enum : std::size_t {
        FNV_offset_basis = 0xcbf29ce484222325ull,
        FNV_prime = 0x100000001b3ull,
      };

      std::size_t res{FNV_offset_basis};

      for (unsigned i{0}; i < 4; ++i) {
        auto const octet{
            static_cast<unsigned char>((flag.value >> (i * 8)) & 0xFF)};
        res ^= octet;
        res *= FNV_prime;
      }

      return res;
    }
  };

private:
  unsigned value;

  friend struct hasher;
};

static constexpr inline category_flag category_flag_none{
    detail::category_flag_none_e::none};

} // namespace cxxet::output
