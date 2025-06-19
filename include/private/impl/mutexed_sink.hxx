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

#include "impl/sink.hxx"

namespace cxxet::impl {

struct mutexed_sink : virtual sink {
  mutexed_sink() noexcept;
  ~mutexed_sink() noexcept override;

  void drain(sink &other) noexcept override final;

private:
  mutexed_sink(mutexed_sink const &) = delete;
  mutexed_sink &operator=(mutexed_sink const &) = delete;
  mutexed_sink(mutexed_sink &&) = delete;
  mutexed_sink &operator=(mutexed_sink &&) = delete;

  std::mutex mtx;

protected:
  std::mutex &get_mutex() noexcept;
};

} // namespace cxxet::impl
