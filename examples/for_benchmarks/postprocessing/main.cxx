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

#include <cstdlib>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include <nlohmann/json.hpp>

#include "../../../include/public/cxxet/timepoint.hxx"

namespace {

bool verbose{false};

long long now() noexcept { return cxxet::impl::as_int_ns(cxxet::impl::now()); }

void log(std::string_view const msg) {
  if (verbose) {
    std::cout << msg << '\n';
  }
}

void log_time_diff(std::string_view const msg, long long const begin,
                   long long const end) {
  if (verbose) {
    std::cout << msg << ": " << static_cast<double>(end - begin) / 1'000'000
              << " [ms]\n";
  }
}

bool ends_with(std::string_view const str, std::string_view const suffix) {
  return (str.size() >= suffix.size()) &&
         (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
}

void process_benchmark(nlohmann::json &target_array,
                       std::filesystem::path const &meta_file_path) {
  log("\tProcessing " + meta_file_path.string());
  auto const meta_json{nlohmann::json::parse(std::ifstream{meta_file_path})};

  auto const benchmark_name{
      meta_json["meta_info"]["benchmark_name"].get<std::string>()};
  auto const traced{meta_json["meta_info"]["traced"].get<std::string>() ==
                    "cxxet"};

  std::filesystem::path const cxxet_results_filename{
      meta_json["meta_info"]["cxxet_results_filename"].get<std::string>()};

  if (auto const is_regular_file{
          std::filesystem::is_regular_file(cxxet_results_filename)};
      traced != is_regular_file) {
    throw '\'' + meta_file_path.string() +
        "' contains inconsistency (traced == " + std::to_string(traced) +
        ", is_regular_file == " + std::to_string(is_regular_file) + ")";
  }

  target_array.push_back(
      {{"benchmark_name", benchmark_name},
       {"traced", traced},
       {"cxxet_results_filename", cxxet_results_filename.string()}});
}

} // namespace

int main(int argc, char const *const *argv) try {
  if ((argc > 1) && ((std::string_view{argv[1]} == "--verbose") ||
                     (std::string_view{argv[1]} == "-v"))) {
    --argc;
    ++argv;
    verbose = true;
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

  auto t0{now()};

  nlohmann::json meta_info = {
      {"cxxet_git_hash",
       nlohmann::json::parse(std::ifstream{
           results_dir / "commit_hash.json"})["context"]["cxxet_git_hash"]
           .get<std::string>()},
  };

  nlohmann::json benchmarks = nlohmann::json::array();

  for (auto const &entry : std::filesystem::directory_iterator{results_dir}) {
    if (!entry.is_regular_file()) {
      continue;
    }

    static constexpr std::string_view meta_file_suffix{"_meta.json"};
    auto const entry_path{entry.path()};
    if (ends_with(entry_path.string(), meta_file_suffix)) {
      process_benchmark(benchmarks, entry_path);
    }
  }

  auto t1{now()};
  log_time_diff("Postprocessed benchmark results", t0, t1);

  nlohmann::json result = {
      {"context", std::move(meta_info)},
      {"benchmarks", std::move(benchmarks)},
  };

  std::ofstream ofs{results_dir / "large.json"};
  ofs << result.dump(2);

  log_time_diff("Saved results into file", t1, now());

  return EXIT_SUCCESS;
} catch (std::exception const &e) {
  std::cout.flush();
  std::cerr << "Failed (3) to postprocess \"large\" benchmark results: "
            << e.what() << std::endl;

  return EXIT_FAILURE;
} catch (std::string const &msg) {
  std::cout.flush();
  std::cerr << "Failed (2) to postprocess \"large\" benchmark results: " << msg
            << std::endl;

  return EXIT_FAILURE;
} catch (char const *const msg) {
  std::cout.flush();
  std::cerr << "Failed (1) to postprocess \"large\" benchmark results: " << msg
            << std::endl;

  return EXIT_FAILURE;
}
