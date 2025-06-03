#pragma once

// TODO how does this behave when linked into another shared library that doesn't want to export those?!
#ifdef __PIC__
#define CXXST_IMPL_API __attribute__((visibility("default")))
#else
#define CXXST_IMPL_API
#endif
