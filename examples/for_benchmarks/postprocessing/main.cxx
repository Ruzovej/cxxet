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

#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>

#include <nlohmann/json.hpp>

#include "log.hxx"
#include "now.hxx"
#include "result_processor.hxx"
#include "str_utils.hxx"

int main(int argc, char const *const *argv) {
  try {
    if ((argc > 1) && ((std::string_view{argv[1]} == "--verbose") ||
                       (std::string_view{argv[1]} == "-v"))) {
      --argc;
      ++argv;
      cxxet_pp::set_verbose(true);
    }
    std::filesystem::path results_dir;
    if (argc > 1) {
      results_dir = argv[1];
    } else {
      throw "missing input directory argument";
    }
    if (!std::filesystem::is_directory(results_dir)) {
      throw "input path '" + results_dir.string() + "' is not a directory";
    }

    auto const t0{cxxet_pp::now()};

    nlohmann::json meta_info = {
        {"cxxet_git_hash",
         nlohmann::json::parse(std::ifstream{
             results_dir / "commit_hash.json"})["context"]["cxxet_git_hash"]
             .get<std::string>()},
    };

    nlohmann::json benchmarks = nlohmann::json::array();

    // TODO parallelize?!
    for (auto const &entry : std::filesystem::directory_iterator{results_dir}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      static constexpr std::string_view meta_file_suffix{"_meta.json"};
      auto const entry_path{entry.path()};
      if (cxxet_pp::ends_with(entry_path.string(), meta_file_suffix)) {
        cxxet_pp::log("\tProcessing " + entry_path.string() + " ...");
        auto const t00{cxxet_pp::now()};
        cxxet_pp::process_benchmark(benchmarks, entry_path);
        cxxet_pp::log_time_diff("\tProcessed " + entry_path.string(), t00,
                                cxxet_pp::now());
      }
    }

    auto const t1{cxxet_pp::now()};
    cxxet_pp::log_time_diff("Postprocessed benchmark results", t0, t1, true);

    std::sort(benchmarks.begin(), benchmarks.end(),
              [](nlohmann::json const &a, nlohmann::json const &b) {
                auto const tpl = [](nlohmann::json const &j) {
                  return std::tie(j["benchmark_name_full"],
                                  j["benchmark_params"]["rep_no"]);
                };
                return tpl(a) < tpl(b);
              });
    cxxet_pp::log_time_diff("Sorted postprocessed benchmark results", t1,
                            cxxet_pp::now(), true);

    nlohmann::json result = {
        {"context", std::move(meta_info)},
        {"benchmarks", std::move(benchmarks)},
    };

    auto const target_file{results_dir / "large.json"};

    std::ofstream ofs{target_file};
    ofs << result.dump(2);

    cxxet_pp::log_time_diff("Saved results into file " + target_file.string(),
                            t1, cxxet_pp::now(), true);

    return EXIT_SUCCESS;
  } catch (std::exception const &e) {
    cxxet_pp::log_error(
        "Failed (3) to postprocess \"large\" benchmark results: " +
        std::string{e.what()});
  } catch (std::string const &msg) {
    cxxet_pp::log_error(
        "Failed (2) to postprocess \"large\" benchmark results: " + msg);
  } catch (char const *const msg) {
    cxxet_pp::log_error(
        "Failed (1) to postprocess \"large\" benchmark results: " +
        std::string{msg});
  } catch (...) {
    cxxet_pp::log_error("Failed (0) to postprocess \"large\" benchmark "
                        "results: unknown exception");
  }
  return EXIT_FAILURE;
}
