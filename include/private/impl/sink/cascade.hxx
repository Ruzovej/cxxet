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

#include "impl/sink/sink_thread_policy_t.hxx"

namespace cxxet::impl::sink {

template <bool thread_safe_v>
struct cascade : sink_thread_policy_t<thread_safe_v> {
  explicit cascade(sink_base *aParent) noexcept
      : base_class_t{}, parent{aParent} {}

  ~cascade() noexcept override { do_flush<true>(); }

  void set_parent(sink_base *aParent) noexcept {
    assert(aParent != this);
    base_class_t::lock();
    parent = aParent;
    base_class_t::unlock();
  }

  void flush() noexcept { do_flush(); }

private:
  template <bool inside_dtor = false> void do_flush() noexcept {
    if (parent) {
      if constexpr (inside_dtor) {
        base_class_t::lock();
      }
      parent->drain(*this);
      if constexpr (inside_dtor) {
        base_class_t::unlock();
      }
    }
  }

  cascade(cascade const &) = delete;
  cascade &operator=(cascade const &) = delete;
  cascade(cascade &&) = delete;
  cascade &operator=(cascade &&) = delete;

  using base_class_t = sink_thread_policy_t<thread_safe_v>;

  sink_base *parent;
};

} // namespace cxxet::impl::sink
