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

#include "cxxet/macros/linkage.h"
#include "cxxet/timepoint.hxx"

namespace cxxet {

struct CXXET_IMPL_API mark_complete {
  inline mark_complete(char const *aDesc) noexcept
      : desc{aDesc}, start{impl::now()} {}

  inline ~mark_complete() noexcept { submit(impl::now()); }

private:
  mark_complete(mark_complete const &) = delete;
  mark_complete &operator=(mark_complete const &) = delete;
  mark_complete(mark_complete &&) = delete;
  mark_complete &operator=(mark_complete &&) = delete;

  void submit(impl::timepoint_t const finish) noexcept;

  const char *const desc;
  impl::timepoint_t const start;
};

} // namespace cxxet
