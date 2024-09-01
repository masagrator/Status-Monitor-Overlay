#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Service s;
} PwmChannelSession;

Result pwmInitialize(void);
void pwmExit(void);
Service* pwmGetServiceSession(void);
Result pwmOpenSession2(PwmChannelSession *out, u32 device_code);
Result pwmChannelSessionGetDutyCycle(PwmChannelSession *c, double* out);
void pwmChannelSessionClose(PwmChannelSession *c);

#ifdef __cplusplus
} // extern "C"
#endif
