#pragma once

#define CXXST_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line) name##line
#define CXXST_IMPL_IMPLICIT_MARKER_NAME(name, line)                            \
  CXXST_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line)
