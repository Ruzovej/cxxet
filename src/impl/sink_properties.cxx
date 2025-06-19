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

#include "impl/sink_properties.hxx"

#include <cstdlib>

#include <functional>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace cxxet::impl {

namespace {

template <typename parse_fn_t, typename default_value_t,
          typename value_t = std::invoke_result_t<parse_fn_t, char const *>>
value_t parse_env_variable(char const *env_var_name, parse_fn_t const &parse_fn,
                           default_value_t const default_value,
                           bool const verbose) {
  auto const value{std::invoke([&]() -> value_t {
    auto const env_var_val{std::getenv(env_var_name)};
    if (env_var_val && *env_var_val != '\0') {
      try {
        return parse_fn(env_var_val);
      } catch (...) {
        // In case of invalid conversion, keep the default value:
      }
    }
    return default_value;
  })};

  if (verbose) {
    std::cout << "Deduced " << env_var_name << ": ";
    if constexpr (std::is_enum_v<value_t>) {
      std::cout << static_cast<std::underlying_type_t<value_t>>(value);
    } else {
      std::cout << value;
    }
    std::cout << '\n';
  }

  return value;
}

bool parse_bool(std::string_view const str_value) {
  return str_value == "1" || str_value == "y" || str_value == "Y" ||
         str_value == "yes" || str_value == "Yes" || str_value == "YES" ||
         str_value == "on" || str_value == "On" || str_value == "ON" ||
         str_value == "true" || str_value == "True" || str_value == "TRUE";
}

output::format parse_output_format(std::string_view const str) {
  if (str == "chrome_trace" || str == "0") {
    return output::format::chrome_trace;
  } else if (str == "raw_naive" || str == "1") {
    return output::format::raw_naive_v0;
  } else {
    throw "Unknown output format";
  }
}

int parse_int(char const *const str_value) {
  return static_cast<int>(std::stoul(str_value));
}

char const *parse_string(char const *const str_value) { return str_value; }

} // namespace

sink_properties::sink_properties() noexcept
    : verbose{parse_env_variable("CXXET_VERBOSE", parse_bool, false, false)},
      target_format{parse_env_variable("CXXET_OUTPUT_FORMAT",
                                       parse_output_format,
                                       output::format::chrome_trace, verbose)},
      default_list_node_capacity{parse_env_variable("CXXET_DEFAULT_BLOCK_SIZE",
                                                    parse_int, 64, verbose)},
      target_filename{parse_env_variable("CXXET_TARGET_FILENAME", parse_string,
                                         nullptr, verbose)} {
  // ...
}

} // namespace cxxet::impl
