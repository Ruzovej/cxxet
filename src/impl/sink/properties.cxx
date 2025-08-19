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

#include "impl/sink/properties.hxx"

#include <cstdlib>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "cxxet/timepoint.hxx"

namespace cxxet::impl::sink {

namespace {

template <typename parse_fn_t, typename default_value_t,
          typename value_t = std::invoke_result_t<parse_fn_t, char const *>>
value_t parse_env_variable(char const *env_var_name, parse_fn_t const &parse_fn,
                           default_value_t const default_value,
                           bool const verbose) {
  auto const value{std::invoke([&]() -> value_t {
    auto const env_var_val{std::getenv(env_var_name)};
    if (env_var_val) {
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
      std::cout << static_cast<long long>(value);
    } else {
      std::cout << value;
    }
    std::cout << '\n';
  }

  return value;
}

bool parse_bool(std::string_view const str_value) {
  if (str_value.empty()) {
    throw std::runtime_error{"Empty string cannot be parsed as bool"};
  } else if (str_value == "1" || str_value == "y" || str_value == "Y" ||
             str_value == "yes" || str_value == "Yes" || str_value == "YES" ||
             str_value == "on" || str_value == "On" || str_value == "ON" ||
             str_value == "true" || str_value == "True" ||
             str_value == "TRUE") {
    return true;
  } else if (str_value == "0" || str_value == "n" || str_value == "N" ||
             str_value == "no" || str_value == "No" || str_value == "NO" ||
             str_value == "off" || str_value == "Off" || str_value == "OFF" ||
             str_value == "false" || str_value == "False" ||
             str_value == "FALSE") {
    return false;
  } else {
    throw std::runtime_error{"Unknown string value for bool"};
  }
}

output::format parse_output_format(std::string_view const str) {
  if (str == "chrome_trace" || str == "0") {
    return output::format::chrome_trace;
  } else if (str == "raw_naive" || str == "1") {
    return output::format::raw_naive_v0;
  } else {
    throw std::runtime_error{"Unknown output format"};
  }
}

int parse_int(char const *const str_value) {
  char *endptr{nullptr};
  auto const ret{static_cast<int>(std::strtol(str_value, &endptr, 10))};
  if ((endptr == str_value) || (*endptr != '\0')) {
    throw std::runtime_error{"Failed to parse integer from string"};
  }
  return ret;
}

char const *parse_string(char const *const str_value) { return str_value; }

long long const static_time_point_zero_ns{as_int_ns(now())};

} // namespace

properties const &properties::instance() noexcept {
  static properties const sink_props_inst{};
  return sink_props_inst;
}

properties::properties() noexcept
    : time_point_zero_ns{static_time_point_zero_ns},
      verbose{parse_env_variable("CXXET_VERBOSE", parse_bool, false, false)},
      default_target_format{
          parse_env_variable("CXXET_OUTPUT_FORMAT", parse_output_format,
                             output::format::chrome_trace, verbose)},
      default_list_node_capacity{parse_env_variable("CXXET_DEFAULT_BLOCK_SIZE",
                                                    parse_int, 64, verbose)},
      default_target_filename{parse_env_variable(
          "CXXET_TARGET_FILENAME", parse_string,
          "/tmp/cxxet_default.pid{pid}.json.XXXXXX", verbose)} {
  // ...
}

} // namespace cxxet::impl::sink

#ifdef CXXET_WITH_UNIT_TESTS

#include <doctest/doctest.h>

namespace cxxet::impl::sink {

TEST_CASE("env. var. parsing for sink::properties") {
  SUBCASE("parsing string to ...") {
    SUBCASE("bool") {
      REQUIRE_EQ(parse_bool("1"), true);
      REQUIRE_EQ(parse_bool("y"), true);
      REQUIRE_EQ(parse_bool("Y"), true);
      REQUIRE_EQ(parse_bool("yes"), true);
      REQUIRE_EQ(parse_bool("Yes"), true);
      REQUIRE_EQ(parse_bool("YES"), true);
      REQUIRE_EQ(parse_bool("on"), true);
      REQUIRE_EQ(parse_bool("On"), true);
      REQUIRE_EQ(parse_bool("ON"), true);
      REQUIRE_EQ(parse_bool("true"), true);
      REQUIRE_EQ(parse_bool("True"), true);
      REQUIRE_EQ(parse_bool("TRUE"), true);

      REQUIRE_EQ(parse_bool("0"), false);
      REQUIRE_EQ(parse_bool("n"), false);
      REQUIRE_EQ(parse_bool("N"), false);
      REQUIRE_EQ(parse_bool("no"), false);
      REQUIRE_EQ(parse_bool("No"), false);
      REQUIRE_EQ(parse_bool("NO"), false);
      REQUIRE_EQ(parse_bool("off"), false);
      REQUIRE_EQ(parse_bool("Off"), false);
      REQUIRE_EQ(parse_bool("OFF"), false);
      REQUIRE_EQ(parse_bool("false"), false);
      REQUIRE_EQ(parse_bool("False"), false);
      REQUIRE_EQ(parse_bool("FALSE"), false);

      REQUIRE_THROWS_AS(parse_bool(""), std::runtime_error);
      REQUIRE_THROWS_AS(parse_bool("is everything else is considered `false` "
                                   "or `true`?! Or none of those?!"),
                        std::runtime_error);
    }

    SUBCASE("output format") {
      REQUIRE_EQ(parse_output_format("chrome_trace"),
                 output::format::chrome_trace);
      REQUIRE_EQ(parse_output_format("0"), output::format::chrome_trace);

      REQUIRE_EQ(parse_output_format("raw_naive"),
                 output::format::raw_naive_v0);
      REQUIRE_EQ(parse_output_format("1"), output::format::raw_naive_v0);

      REQUIRE_THROWS_AS(parse_output_format(""), std::runtime_error);
      REQUIRE_THROWS_AS(parse_output_format("unknown"), std::runtime_error);
      REQUIRE_THROWS_AS(parse_output_format("Chrome_trace"),
                        std::runtime_error);
      REQUIRE_THROWS_AS(parse_output_format("-1"), std::runtime_error);
      REQUIRE_THROWS_AS(parse_output_format("2"), std::runtime_error);
    }

    // how to properly test "identities"?!
    SUBCASE("int") {
      REQUIRE_EQ(parse_int("0"), 0);
      REQUIRE_EQ(parse_int("1"), 1);
      REQUIRE_EQ(parse_int("6008"), 6008);
      REQUIRE_EQ(parse_int("-1"), -1);
      REQUIRE_EQ(parse_int("-42"), -42);

      REQUIRE_THROWS_AS(parse_int(""), std::runtime_error);
      REQUIRE_THROWS_AS(parse_int("1.1"), std::runtime_error);
      REQUIRE_THROWS_AS(parse_int("1."), std::runtime_error);
      REQUIRE_THROWS_AS(parse_int("1,"), std::runtime_error);
      REQUIRE_THROWS_AS(parse_int("1a"), std::runtime_error);
    }

    SUBCASE("string") {
      REQUIRE_EQ(std::string_view{parse_string("")}, "");
      REQUIRE_EQ(std::string_view{parse_string("1")}, "1");
      REQUIRE_EQ(std::string_view{parse_string("abc")}, "abc");
    }
  }

  SUBCASE("parsing env. var(s)") {
    char const *const test_env_var_name{"CXXET_TEST_ENV_VAR"};

    auto const parse = [test_env_var_name](auto const &parse_fn,
                                           auto const default_value) {
      return parse_env_variable(test_env_var_name, parse_fn, default_value,
                                false);
    };

    REQUIRE_EQ(unsetenv(test_env_var_name), 0);

    SUBCASE("unset") {
      REQUIRE_EQ(parse(parse_bool, false), false);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::chrome_trace),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 42);
      REQUIRE_EQ(parse(parse_int, 0), 0);

      REQUIRE_EQ(parse(parse_string, nullptr), nullptr);
      REQUIRE_EQ(parse(parse_string, ""), "");
      REQUIRE_EQ(parse(parse_string, "default"), "default");
    }

    SUBCASE("empty") {
      REQUIRE_EQ(setenv(test_env_var_name, "", 1), 0);

      REQUIRE_EQ(parse(parse_bool, false), false);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::chrome_trace),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 42);
      REQUIRE_EQ(parse(parse_int, 0), 0);

      REQUIRE_EQ(std::string_view{parse(parse_string, nullptr)}, "");
      REQUIRE_EQ(std::string_view{parse(parse_string, "default")}, "");
    }

    SUBCASE("bool (true)") {
      REQUIRE_EQ(setenv(test_env_var_name, "true", 1), 0);

      REQUIRE_EQ(parse(parse_bool, false), true);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::chrome_trace),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 42);

      REQUIRE_EQ(std::string_view{parse(parse_string, "default")}, "true");
    }

    SUBCASE("output format (chrome_trace)") {
      REQUIRE_EQ(setenv(test_env_var_name, "chrome_trace", 1), 0);

      REQUIRE_EQ(parse(parse_bool, false), false);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::raw_naive_v0),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 42);

      REQUIRE_EQ(std::string_view{parse(parse_string, "default")},
                 "chrome_trace");
    }

    SUBCASE("int (123)") {
      REQUIRE_EQ(setenv(test_env_var_name, "123", 1), 0);

      REQUIRE_EQ(parse(parse_bool, false), false);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::chrome_trace),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 123);

      REQUIRE_EQ(std::string_view{parse(parse_string, "default")}, "123");
    }

    SUBCASE("string (ahoj)") {
      REQUIRE_EQ(setenv(test_env_var_name, "ahoj", 1), 0);

      REQUIRE_EQ(parse(parse_bool, false), false);
      REQUIRE_EQ(parse(parse_bool, true), true);

      REQUIRE_EQ(parse(parse_output_format, output::format::chrome_trace),
                 output::format::chrome_trace);

      REQUIRE_EQ(parse(parse_int, 42), 42);

      REQUIRE_EQ(std::string_view{parse(parse_string, "default")}, "ahoj");
    }

    REQUIRE_EQ(unsetenv(test_env_var_name), 0);
  }
}

} // namespace cxxet::impl::sink

#endif
