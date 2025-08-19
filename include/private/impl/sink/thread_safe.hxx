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

#include <mutex>

#include "impl/sink/sink_base.hxx"

namespace cxxet::impl::sink {

struct thread_safe : sink_base {
  explicit thread_safe() noexcept;
  ~thread_safe() noexcept override;

  void drain(sink_base &other) noexcept override final;

  void lock() noexcept;
  void unlock() noexcept;

protected:
  std::mutex mtx;

private:
  thread_safe(thread_safe const &) = delete;
  thread_safe &operator=(thread_safe const &) = delete;
  thread_safe(thread_safe &&) = delete;
  thread_safe &operator=(thread_safe &&) = delete;
};

} // namespace cxxet::impl::sink
