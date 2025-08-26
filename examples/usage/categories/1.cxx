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

#include <chrono>
#include <iostream>
#include <thread>

#include "cxxet/basic.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/output/categories.hxx"
#endif

#ifdef CXXET_ENABLE
auto const network_category{cxxet::output::register_category_name("network")};
auto const database_category{cxxet::output::register_category_name("database")};
auto const background_category{
    cxxet::output::register_category_name(1024, "background-tasks")};
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char const **argv) {
  CXXET_sink_thread_reserve();
  CXXET_sink_global_set_flush_target(argc > 1 ? argv[1] : "/dev/stdout");

  {
    CXXET_mark_complete(network_category, "1 HTTP request to API");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  {
    CXXET_mark_complete(database_category, "2 Query user data");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  {
    // Combine multiple categories (bitwise OR)
    CXXET_mark_complete(network_category | database_category,
                        "3 Sync data to server");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  {
    CXXET_mark_complete(background_category, "4 Cleanup temporary files");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  {
    // Mark without category
    CXXET_mark_complete("5 Uncategorized operation (default)");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  {
#ifdef CXXET_ENABLE
    auto const unused_categories{(1u << 5) | (1u << 7) | (1u << 13) |
                                 (1u << 19) | (1u << 23)};

    if ((unused_categories & network_category) ||
        (unused_categories & database_category) ||
        (unused_categories & background_category)) {
      throw std::runtime_error("Test setup error");
    }
#endif

    // Mark with invalid categories
    CXXET_mark_complete(
        unused_categories,
        "6 Uncategorized operation (ignoring unknown categories)");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  return 0;
}
