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

#pragma once

#include <memory>
#include <string>

#include "cxxet/macros/linkage.h"
#include "cxxet/output/writer.hxx"

namespace cxxet {

struct CXXET_IMPL_API sink_handle {
  virtual ~sink_handle() noexcept;

  virtual void divert_thread_sink_to_this() noexcept = 0;
};

struct CXXET_IMPL_API file_sink_handle : sink_handle {
  [[nodiscard]] static std::unique_ptr<file_sink_handle>
  make(bool const thread_safe) noexcept;

  virtual void set_flush_target(std::string filename) noexcept = 0;
  virtual void
  set_flush_target(std::unique_ptr<output::writer> custom_writer) noexcept = 0;
  void set_flush_target(std::nullptr_t) noexcept = delete;

  [[nodiscard]] virtual unsigned
  register_category_name(unsigned const category, std::string name,
                         bool const allow_rename = false) noexcept = 0;
  [[nodiscard]] inline unsigned
  register_category_name(std::string name,
                         bool const allow_rename = false) noexcept {
    return register_category_name(0, std::move(name), allow_rename);
  }
};

struct CXXET_IMPL_API cascade_sink_handle : sink_handle {
  [[nodiscard]] static std::unique_ptr<cascade_sink_handle>
  make(bool const thread_safe, sink_handle &parent) noexcept;

  virtual void flush_now() noexcept = 0;
};

} // namespace cxxet
