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

#include "impl/cascade_sink.hxx"

namespace cxxet::impl {

struct thread_sink : cascade_sink {
  explicit thread_sink(sink *aParent) noexcept;
  ~thread_sink() noexcept override;

  void append_event(event::any const &evt) noexcept;

  void reserve(int const minimum_free_capacity) noexcept;

private:
  thread_sink(thread_sink const &) = delete;
  thread_sink &operator=(thread_sink const &) = delete;
  thread_sink(thread_sink &&) = delete;
  thread_sink &operator=(thread_sink &&) = delete;

  int default_node_capacity{};
};

} // namespace cxxet::impl
