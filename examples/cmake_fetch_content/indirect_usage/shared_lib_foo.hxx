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

#ifdef __PIC__
#define SHARED_LIB_FOO_API __attribute__((visibility("default")))
#else
#define SHARED_LIB_FOO_API
#endif

namespace shared_lib_foo {

SHARED_LIB_FOO_API void init() noexcept;
SHARED_LIB_FOO_API void flush(char const *const filename) noexcept;

SHARED_LIB_FOO_API void perform_work(unsigned const i) noexcept;

struct SHARED_LIB_FOO_API some_class {
  explicit some_class(
      int const ii); // only even & positive args are allowed, throws
                     // `std::exception` derived thing otherwise

  void work() noexcept;

private:
  unsigned i;
};

SHARED_LIB_FOO_API some_class make_some_class(int const i);

} // namespace shared_lib_foo
