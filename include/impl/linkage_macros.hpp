#pragma once

#ifdef __PIC__
#define RSM_API __attribute__((visibility("default")))
#else
#define RSM_API
#endif
