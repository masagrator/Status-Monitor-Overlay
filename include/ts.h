#pragma once

typedef struct {
    Service s;
} tsSession;

/// Location
typedef enum {
    TsDeviceCode_Internal = 0x41000001,    ///< TMP451 Internal: PCB
    TsDeviceCode_External = 0x41000002,    ///< TMP451 External: SoC
} tsDeviceCode;

#ifdef __cplusplus
extern "C" {
#endif

Result tsOpenTsSession(Service* serviceSession, tsSession* out, tsDeviceCode device_code);

void tsCloseTsSession(tsSession* in);

Result tsGetTemperatureWithTsSession(tsSession *in, float* temperature);

#ifdef __cplusplus
}
#endif