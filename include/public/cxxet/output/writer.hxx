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

#include <string_view>

// #include "cxxet/macros/linkage.h" // TODO #98 is this needed?!

namespace cxxet::output {

// struct CXXET_IMPL_API writer {
struct writer {
  writer() = default;
  virtual ~writer() noexcept = default;

  template <typename T> writer &operator<<(T const &value) {
    write(value);
    return *this;
  }

  virtual void prepare_for_writing() = 0;

  virtual void write(std::string_view const data) = 0;
  virtual void write(long long const ll) = 0;
  virtual void write(unsigned long long const ull) = 0;
  virtual void write(double const dp) = 0;

  virtual void finalize_and_flush() = 0;

private:
  writer(writer const &) = delete;
  writer &operator=(writer const &) = delete;
  writer(writer &&) = delete;
  writer &operator=(writer &&) = delete;
};

template <>
inline writer &writer::operator<< <const char *>(char const *const &value) {
  write(std::string_view{value});
  return *this;
}

template <> inline writer &writer::operator<< <char>(char const &value) {
  write(std::string_view{&value, 1});
  return *this;
}

template <> inline writer &writer::operator<< <int>(int const &value) {
  write(static_cast<long long>(value));
  return *this;
}

template <>
inline writer &writer::operator<< <unsigned>(unsigned const &value) {
  write(static_cast<unsigned long long>(value));
  return *this;
}

} // namespace cxxet::output
