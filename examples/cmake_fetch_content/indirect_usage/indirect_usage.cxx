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

#include <thread>

#include <cxxet/all.hxx>

#include "shared_lib_foo.hxx"

int main(int argc, char const **argv) {
  CXXET_mark_complete("main");
  shared_lib_foo::init(); // `CXXET_sink_thread_reserve()` is hidden inside this
                          // ... in real world, it should be done somehow
                          // cleaner (or at least be properly documented, ...)

  std::thread t{[]() {
    CXXET_mark_complete("other thread ...");
    shared_lib_foo::init(); // ditto
    shared_lib_foo::some_class sc{8};
    sc.work();
  }};

  shared_lib_foo::some_class sc{6};
  sc.work();

  t.join();

  shared_lib_foo::flush(argc > 1 ? argv[1] : "/dev/stdout");
  // contains immediate flush via `CXXET_sink_global_set_flush_target(...,
  // filename, ...);`

  return 0;
}
