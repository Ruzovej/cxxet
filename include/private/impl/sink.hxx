/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "cxxet/output_format.hxx"
#include "impl/event/list/list.hxx"

namespace cxxet::impl {

struct sink {
  sink() noexcept;
  virtual ~sink() noexcept;

  void flush_to_file(long long const time_point_zero,
                     cxxet::output::format const fmt,
                     char const *const filename) noexcept;

  virtual void drain(sink &other) noexcept;

protected:
  event::list events;

private:
  sink(sink const &) = delete;
  sink &operator=(sink const &) = delete;
  sink(sink &&) = delete;
  sink &operator=(sink &&) = delete;
};

} // namespace cxxet::impl
