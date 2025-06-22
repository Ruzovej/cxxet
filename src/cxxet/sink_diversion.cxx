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

#include "cxxet/sink_diversion.hxx"

//#include "impl/cascade_sink_thread_safe.hxx"
//#include "impl/file_sink.hxx"
//#include "impl/sink.hxx"

namespace cxxet {

sink_handle::~sink_handle() noexcept = default;

std::unique_ptr<file_sink_handle>
file_sink_handle::make(bool const thread_safe) noexcept {
  // TODO implement
  (void)thread_safe;
  return nullptr;
}

std::unique_ptr<file_sink_handle>
file_sink_handle::make(bool const thread_safe, output::format const aFmt,
                       char const *const aTarget_filename) noexcept {
  (void)thread_safe;
  (void)aFmt;
  (void)aTarget_filename;
  return nullptr;
}

file_sink_handle::~file_sink_handle() noexcept = default;

std::unique_ptr<cascade_sink_handle> cascade_sink_handle::make(
    bool const thread_safe,
    std::unique_ptr<sink_handle> const &aParent) noexcept {
  (void)thread_safe;
  (void)aParent;
  return nullptr;
}

cascade_sink_handle::~cascade_sink_handle() noexcept = default;

} // namespace cxxet
