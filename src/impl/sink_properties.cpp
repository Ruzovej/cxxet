#include "impl/sink_properties.hpp"

#include <cstdlib>

#include <functional>
#include <iostream>
#include <string_view>
#include <type_traits>

namespace rsm::impl {

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

char const *parse_string(char const *str_value) { return str_value; }

} // namespace

sink_properties::sink_properties() noexcept
    : verbose{parse_env_variable("RSM_VERBOSE", parse_bool, false, false)},
      target_format{parse_env_variable("RSM_OUTPUT_FORMAT", parse_output_format,
                                       output::format::chrome_trace, verbose)},
      default_list_node_capacity{
          parse_env_variable("RSM_DEFAULT_BLOCK_SIZE", parse_int, 64, verbose)},
      target_filename{parse_env_variable("RSM_TARGET_FILENAME", parse_string,
                                         nullptr, verbose)} {
  // ...
}

} // namespace rsm::impl
