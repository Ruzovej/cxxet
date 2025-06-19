/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#define CXXET_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line) name##line
#define CXXET_IMPL_IMPLICIT_MARKER_NAME(name, line)                            \
  CXXET_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line)
