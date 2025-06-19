#pragma once

#define CXXET_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line) name##line
#define CXXET_IMPL_IMPLICIT_MARKER_NAME(name, line)                            \
  CXXET_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line)
