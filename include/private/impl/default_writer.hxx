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

#include <fstream>
#include <string_view>

#include "cxxet/output/writer.hxx"

namespace cxxet::impl {

struct default_writer final : output::writer {
  explicit default_writer(char const *const aTarget_filename);
  ~default_writer() noexcept override;

  void prepare_for_writing() override;

  void write(std::string_view const data) override;
  void write(long long const ll) override;
  void write(unsigned long long const ull) override;
  void write(double const dp) override;

  void finalize_and_flush() override;

private:
  char const *const target_filename;
  std::ofstream file_stream;
};

} // namespace cxxet::impl
