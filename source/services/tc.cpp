#include <switch.h>
#include "services/tc.hpp"

static Service g_tcSrv;


Result _tcCmdNoInOut32(u32 *out, u8 cmd_id) {
    return serviceDispatchOut(&g_tcSrv, cmd_id, *out);
}

Result tcGetTemperatureMilliC(s32 *temperature) {
    return _tcCmdNoInOut32((u32*)temperature, 9);
}

Result tcInitialize(void) {
    return smGetService(&g_tcSrv, "tc");;
}

void tcExit(void) {
	serviceClose(&g_tcSrv);
}