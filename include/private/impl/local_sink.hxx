/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "impl/sink.hxx"

namespace cxxet::impl {

struct local_sink : sink {
  explicit local_sink(sink *aParent) noexcept;
  ~local_sink() noexcept override;

  void append_event(event::any const &evt) noexcept;

  void flush() noexcept;

  void reserve(int const minimum_free_capacity) noexcept;

private:
  local_sink(local_sink const &) = delete;
  local_sink &operator=(local_sink const &) = delete;
  local_sink(local_sink &&) = delete;
  local_sink &operator=(local_sink &&) = delete;

  sink *parent;
  int default_node_capacity{};
};

} // namespace cxxet::impl
