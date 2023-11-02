#pragma once

#include <stdint.h>

typedef enum
{
    SysClkEmcLoad_All = 0,
    SysClkEmcLoad_Cpu,
    SysClkEmcLoad_EnumMax
} SysClkEmcLoadType;

typedef struct {
    uint32_t load[SysClkEmcLoad_EnumMax];
} SysClkEmcLoad;