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

#include "str_utils.hxx"

namespace cxxet_pp {

bool begins_with(std::string_view const str, std::string_view const prefix) {
  return (str.size() >= prefix.size()) &&
         (str.compare(0, prefix.size(), prefix) == 0);
}

bool ends_with(std::string_view const str, std::string_view const suffix) {
  return (str.size() >= suffix.size()) &&
         (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
}

} // namespace cxxet_pp
