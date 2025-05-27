#pragma once

#ifdef __PIC__
#define RSM_IMPL_API __attribute__((visibility("default")))
#else
#define RSM_IMPL_API
#endif
