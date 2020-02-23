#include "services/fan.hpp"

static Service g_fanSrv;
static Service g_fanCtl;

static Result _fanOpenController(void);

Result fanInitialize(void) {
    Result rc = smGetService(&g_fanSrv, "fan");
    
    if (R_SUCCEEDED(rc)) rc = _fanOpenController();

    return rc;
}

void fanExit(void) {
    serviceClose(&g_fanSrv);
    serviceClose(&g_fanCtl);
}

Result _fanOpenController(void) {
    u32 in = 0x3d000001;
    return serviceDispatchIn(&g_fanSrv, 0, in,
        .out_num_objects = 1,
        .out_objects = &g_fanCtl
    );
}

Service* fanGetServiceSession(void) {
    return &g_fanSrv;
}

Service* fanGetServiceSession_Controller(void) {
    return &g_fanCtl;
}

Result fanSetRotationSpeedLevel(float level) {
    return serviceDispatchIn(&g_fanCtl, 0, level);
}

Result fanGetRotationSpeedLevel(float *level) {
    return serviceDispatchOut(&g_fanCtl, 2, *level);
}
