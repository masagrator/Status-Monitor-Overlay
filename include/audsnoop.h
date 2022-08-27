#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

Result audsnoopInitialize(void);
void audsnoopExit(void);

Result audsnoopEnableDspUsageMeasurement(void);
Result audsnoopDisableDspUsageMeasurement(void);

Result audsnoopGetDspUsage(u32 *usage);

#ifdef __cplusplus
} // extern "C"
#endif
