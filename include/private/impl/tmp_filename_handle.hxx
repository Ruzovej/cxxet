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

#include <array>
#include <string_view>

namespace cxxet::impl {

struct tmp_filename_handle {
  static bool valid_base(char const *const aBase) noexcept;

  explicit tmp_filename_handle(char const *const aBase) noexcept;
  ~tmp_filename_handle() noexcept;

  explicit operator char const *();
  explicit operator std::string_view();

private:
  char const *base;
#if defined(_WIN32)
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  static constexpr unsigned buffer_size{256};
  std::array<char, buffer_size> buffer;
#else
#error "Unsupported platform"
#endif
};

} // namespace cxxet::impl
