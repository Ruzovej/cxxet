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
#include "cxxet/output/category_flag.hxx"
#include "cxxet/timepoint.hxx"

namespace cxxet::mark {

struct CXXET_IMPL_API complete {
  inline complete(output::category_flag const aCategories,
                  char const *const aDesc) noexcept
      : categories{aCategories}, desc{aDesc}, start{impl::now()} {}
  inline explicit complete(char const *const aDesc) noexcept
      : complete{output::category_flag_none, aDesc} {}

  inline ~complete() noexcept { submit(impl::now()); }

private:
  complete(complete const &) = delete;
  complete &operator=(complete const &) = delete;
  complete(complete &&) = delete;
  complete &operator=(complete &&) = delete;

  void submit(impl::timepoint_t const finish) noexcept;

  output::category_flag const categories;
  const char *const desc;
  impl::timepoint_t const start;
};

} // namespace cxxet::mark
