#define NX_SERVICE_ASSUME_NON_DOMAIN
#include <switch.h>
#include "service_guard.h"

static Service g_audsnoopSrv;

NX_GENERATE_SERVICE_GUARD(audsnoop);

Result _audsnoopInitialize(void) {
    return smGetService(&g_audsnoopSrv, "auddev");
}

void _audsnoopCleanup(void) {
    serviceClose(&g_audsnoopSrv);
}

Service* audsnoopGetServiceSession(void) {
    return &g_audsnoopSrv;
}

Result audsnoopEnableDspUsageMeasurement(void) {
    return serviceDispatch(&g_audsnoopSrv, 0);
}

Result audsnoopDisableDspUsageMeasurement(void) {
    return serviceDispatch(&g_audsnoopSrv, 1);
}

Result audsnoopGetDspUsage(u32 *usage) {
    u32 tmp;
    Result rc = serviceDispatchOut(&g_audsnoopSrv, 6, tmp);
    if (R_SUCCEEDED(rc) && usage)
        *usage = tmp;
    return rc;
}
