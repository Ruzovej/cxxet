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

#include "impl/default_writer.hxx"

#include <iostream>

namespace cxxet::impl {

default_writer::default_writer(char const *const target_filename) : writer{} {
  if (target_filename != nullptr && target_filename[0] != '\0') {
    if (target_filename == std::string_view{"/dev/stdout"}) {
      std::cout.flush();
      file_stream.open(target_filename, std::ios::app);
    } else {
      file_stream.open(target_filename, std::ios::out);
    }
    if (!file_stream.is_open()) {
      throw std::runtime_error("Failed to open file: " +
                               std::string{target_filename});
    }
  } else {
    throw std::runtime_error("Failed to open file: nullptr or empty c-string");
  }
}

default_writer::~default_writer() noexcept = default;

void default_writer::write(std::string_view const data) { file_stream << data; }

void default_writer::write(long long const ll) { file_stream << ll; }

void default_writer::write(unsigned long long const ull) { file_stream << ull; }

void default_writer::write(double const dp) { file_stream << dp; }

void default_writer::finalize_and_flush() {
  if (file_stream.is_open()) {
    file_stream.flush();
    file_stream.close();
  }
}

} // namespace cxxet::impl
