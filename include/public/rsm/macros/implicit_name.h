#pragma once

#define RSM_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line) name##line
#define RSM_IMPL_IMPLICIT_MARKER_NAME(name, line)                              \
  RSM_IMPL_IMPLICIT_MARKER_NAME_JOIN(name, line)
