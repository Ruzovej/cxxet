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

#include "cxxet/output/categories.hxx"

#include "impl/sink/file_sink.hxx"

namespace cxxet::output {

category_flag register_category_name(category_flag const category,
                                     std::string name,
                                     bool const allow_rename) {
  return impl::sink::file_sink_global_instance().register_category_name(
      category, std::move(name), allow_rename);
}

} // namespace cxxet::output
