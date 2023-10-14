#include "switch.h"
#include "ts.h"

Result tsOpenTsSession(Service* serviceSession, tsSession* out, tsDeviceCode device_code) {
    return serviceDispatchIn(serviceSession, 4, device_code,
        .out_num_objects = 1,
        .out_objects = &out->s,
    );
}

void tsCloseTsSession(tsSession* in) {
    serviceClose(&in->s);
}

Result tsGetTemperatureWithTsSession(tsSession *ITs, float* temperature) {
    return serviceDispatchOut(&ITs->s, 4, *temperature);
}