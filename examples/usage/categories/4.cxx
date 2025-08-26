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

#include "cxxet/basic.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/output/categories.hxx"
#endif

#ifdef CXXET_ENABLE
auto const first_category{
    cxxet::output::register_category_name("first_category")};

// It's questionable whether overwriting previously set values is a good
// practice ...:
auto const first_category_overwritten{cxxet::output::register_category_name(
    first_category, "first_category_overwritten", true)};

// Duplicate flag: same as the prev. one, fails:
auto const duplicate_category{cxxet::output::register_category_name(
    first_category, "duplicate_category")};
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_thread_reserve();
  CXXET_sink_global_set_flush_target(argc > 1 ? argv[1] : "/dev/stdout");

  CXXET_mark_instant(duplicate_category, "some recorded event ...");

  return 0;
}