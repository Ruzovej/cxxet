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

#include "impl/event/list/list.hxx"

namespace cxxet::impl::sink {

struct sink_base {
  sink_base() noexcept;
  virtual ~sink_base() noexcept;

  virtual void drain(sink_base &other) noexcept = 0;

  bool has_events() const noexcept;

protected:
  event::list events;

  void do_drain(sink_base &other) noexcept;

private:
  sink_base(sink_base const &) = delete;
  sink_base &operator=(sink_base const &) = delete;
  sink_base(sink_base &&) = delete;
  sink_base &operator=(sink_base &&) = delete;
};

} // namespace cxxet::impl::sink
