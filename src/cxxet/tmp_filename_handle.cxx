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

#include "impl/tmp_filename_handle.hxx"

#if defined(_WIN32)
#include <windows.h>
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#else
#error "Unsupported platform"
#endif

#include <cassert>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

namespace cxxet::impl {

bool tmp_filename_handle::valid_base(char const *const aBase) noexcept {
  std::string_view const base{aBase};
#if defined(_WIN32)
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  static constexpr int num_required_Xs{6};
  return (base.size() < buffer_size) && (base.size() >= num_required_Xs) &&
         std::all_of(base.crbegin(), base.crbegin() + num_required_Xs,
                     [](char const c) { return c == 'X'; });
#else
#error "Unsupported platform"
#endif
}

tmp_filename_handle::tmp_filename_handle(char const *const aBase) noexcept
    : base{aBase} {
  buffer[0] = '\0';
  assert(valid_base(base));
}

tmp_filename_handle::~tmp_filename_handle() noexcept {
#if defined(_WIN32)
#include <windows.h>
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
// nothing
#else
#error "Unsupported platform"
#endif
}

tmp_filename_handle::operator char const *() {
#if defined(_WIN32)
#include <windows.h>
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
  if (buffer[0] == '\0') {
    // is there a better & easier way?
    std::copy(base, base + std::strlen(base) + 1, buffer.data());
    int fd;
    // https://man7.org/linux/man-pages/man3/mkstemp.3.html
    if ((fd = mkstemp(buffer.data())) == -1) {
      throw std::runtime_error{
          std::string{"'mkstemp' failed to create temporary file out of '"} +
          base + "', errno = " + std::to_string(errno) + " -> " +
          strerror(errno)};
    }
    close(fd); // keeping just name of this created file is enough
  }

#else
#error "Unsupported platform"
#endif

  return buffer.data();
}

tmp_filename_handle::operator std::string_view() {
  return std::string_view{static_cast<char const *>(*this)};
}

} // namespace cxxet::impl

#ifdef CXXET_WITH_UNIT_TESTS

#include <filesystem>
#include <fstream>

#include <doctest/doctest.h>

namespace cxxet::impl {

TEST_CASE("tmp_filename_handle") {
  SUBCASE("valid base") {
#if defined(_WIN32)
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    REQUIRE(tmp_filename_handle::valid_base("XXXXXX"));
    REQUIRE(tmp_filename_handle::valid_base("XXXXXXX"));
    REQUIRE(tmp_filename_handle::valid_base("some_local_file.XXXXXX"));
    REQUIRE(tmp_filename_handle::valid_base("/tmp/XXXXXX"));
#else
#error "Unsupported platform"
#endif
  }

  SUBCASE("invalid base") {
#if defined(_WIN32)
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    REQUIRE(!tmp_filename_handle::valid_base("X"));
    REQUIRE(!tmp_filename_handle::valid_base("XX"));
    REQUIRE(!tmp_filename_handle::valid_base("XXX"));
    REQUIRE(!tmp_filename_handle::valid_base("XXXX"));
    REQUIRE(!tmp_filename_handle::valid_base("XXXXX"));
    REQUIRE(!tmp_filename_handle::valid_base("/tmp/XXXXXX/"));
#else
#error "Unsupported platform"
#endif
  }

  SUBCASE("writable files") {
    std::string base{};
#if defined(_WIN32)
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    auto const tmp_base{std::getenv("TMP_RESULT_DIR_BASE")};
    REQUIRE_NE(tmp_base, nullptr);
    base = tmp_base + std::string{"/tmp_file.XXXXXX"};
    REQUIRE(tmp_filename_handle::valid_base(base.c_str()));
#else
#error "Unsupported platform"
#endif
    tmp_filename_handle handle1{base.c_str()};
    tmp_filename_handle handle2{base.c_str()};
    tmp_filename_handle handle3{base.c_str()};

    REQUIRE_NOTHROW((void)static_cast<std::string_view>(handle1));
    REQUIRE_NOTHROW((void)static_cast<std::string_view>(handle2));
    REQUIRE_NOTHROW((void)static_cast<std::string_view>(handle3));

    REQUIRE(std::filesystem::exists(
        std::filesystem::path{static_cast<std::string_view>(handle1)}));
    REQUIRE(std::filesystem::exists(
        std::filesystem::path{static_cast<std::string_view>(handle2)}));
    REQUIRE(std::filesystem::exists(
        std::filesystem::path{static_cast<std::string_view>(handle3)}));

    REQUIRE_NE(std::string_view{static_cast<std::string_view>(handle1)},
               std::string_view{static_cast<std::string_view>(handle2)});
    REQUIRE_NE(std::string_view{static_cast<std::string_view>(handle1)},
               std::string_view{static_cast<std::string_view>(handle3)});
    REQUIRE_NE(std::string_view{static_cast<std::string_view>(handle2)},
               std::string_view{static_cast<std::string_view>(handle3)});

    auto const write_to_file = [](tmp_filename_handle &handle,
                                  decltype(std::ios::out) const flags,
                                  std::string_view const content) {
      std::ofstream ofs{static_cast<char const *>(handle), flags};
      ofs << content;
      ofs.close();
    };

    REQUIRE_NOTHROW(write_to_file(handle1, std::ios::out, "test1"));
    REQUIRE_NOTHROW(write_to_file(handle2, std::ios::out, "test2.1"));
    REQUIRE_NOTHROW(write_to_file(handle2, std::ios::app, "-append-test2.2"));

    auto const read_file = [](tmp_filename_handle &handle) {
      std::ifstream ifs{static_cast<char const *>(handle)};
      std::string content{};
      ifs >> content;
      return content;
    };

    REQUIRE_EQ(read_file(handle1), std::string_view{"test1"});
    REQUIRE_EQ(read_file(handle2), std::string_view{"test2.1-append-test2.2"});
    REQUIRE_EQ(read_file(handle3), std::string_view{""});
  }
}

} // namespace cxxet::impl

#endif
