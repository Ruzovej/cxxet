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

#include <filesystem>
#include <string>
#include <string_view>

#include "compare.hxx"
#include "log.hxx"
#include "now.hxx"

int main(int const argc, char const *const *const argv) {
  try {
    auto const usage = [&]() {
      cxxet_pp::log(
          "usage: " + std::string{argv[0]} +
              " [-v|--verbose] [-h|--help] [-c|--compact] "
              "[-o|--out|--out-json <output_file>] "
              "<input_file_baseline> <input_file_challenger>\n\tproviding the "
              "same input file twice means comparing the overhead of the "
              "'cxxet' over 'bare' version",
          true);
    };
    auto consume_arg = [&usage, argc{argc - 1}, argv{argv + 1}](
                           bool const require = false,
                           std::string_view const missing_error_context_desc =
                               "") mutable -> std::string_view {
      if (argc <= 0) {
        if (require) {
          usage();
          throw "missing argument (" + std::string{missing_error_context_desc} +
              ')';
        }
        return "";
      }
      auto const arg{*argv};
      --argc;
      ++argv;
      return arg;
    };

    std::string_view output;
    int json_indent{2};
    std::string_view input_baseline_filename;
    std::string_view input_challenger_filename;

    while (true) { // why are there no init-statements for `while` loops?!
      auto const arg{consume_arg()};
      if (arg.empty()) {
        break;
      }

      if (arg == "-v" || arg == "--verbose") {
        cxxet_pp::set_verbose(
            true); // ignore repetitions - use the latest value
      } else if (arg == "-h" || arg == "--help") {
        usage();
        return EXIT_SUCCESS;
      } else if (arg == "-o" || arg == "--out" || arg == "--out-json") {
        output =
            consume_arg(true, arg); // ignore repetitions - use the latest value
      } else if (arg == "-c" || arg == "--compact") {
        json_indent = -1;
      } else {
        if (input_baseline_filename.empty()) {
          input_baseline_filename = arg;
        } else if (input_challenger_filename.empty()) {
          input_challenger_filename = arg;
        } else {
          throw "unexpected extra argument '" + std::string{arg} + "'";
        }
      }
    }

    if (input_baseline_filename.empty() || input_challenger_filename.empty()) {
      throw "missing input file(s)";
    }

    std::filesystem::path const input_baseline{input_baseline_filename};
    std::filesystem::path const input_challenger{input_challenger_filename};

    if (!std::filesystem::exists(input_baseline) ||
        !std::filesystem::exists(input_challenger)) {
      throw "input file(s) do not exist";
    }

    auto const t0{cxxet_pp::now()};

    cxxet_cmp::compare_files(input_baseline, input_challenger, output,
                             json_indent);

    cxxet_pp::log_time_diff("Compared postprocessed benchmark results", t0,
                            cxxet_pp::now(), true);

    return EXIT_SUCCESS;
  } catch (std::exception const &e) {
    cxxet_pp::log_error("Failed (3) to compare \"large\" benchmark results: " +
                        std::string{e.what()});
  } catch (std::string const &msg) {
    cxxet_pp::log_error("Failed (2) to compare \"large\" benchmark results: " +
                        msg);
  } catch (char const *const msg) {
    cxxet_pp::log_error("Failed (1) to compare \"large\" benchmark results: " +
                        std::string{msg});
  } catch (...) {
    cxxet_pp::log_error("Failed (0) to compare \"large\" benchmark "
                        "results: unknown exception");
  }
  return EXIT_FAILURE;
}
