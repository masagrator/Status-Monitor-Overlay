#pragma once
#include <switch.h>

Result tcInitialize(void);
void tcExit(void);
Result _tcCmdNoInOut32(u32 *out, u8 cmd_id);
Result tcGetTemperatureMilliC(s32 *temperature);