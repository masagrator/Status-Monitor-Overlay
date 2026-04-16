/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */
 
/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */


#pragma once

#include <stdint.h>
#include "board.h"

typedef struct
{
    uint64_t applicationId;
    HocClkProfile profile;
    uint32_t freqs[HocClkModule_EnumMax];
    uint32_t realFreqs[HocClkModule_EnumMax];
    uint32_t overrideFreqs[HocClkModule_EnumMax];
    uint32_t temps[HocClkThermalSensor_EnumMax];
    int32_t power[HocClkPowerSensor_EnumMax];
    uint32_t partLoad[HocClkPartLoad_EnumMax];
    uint32_t voltages[HocClkVoltage_EnumMax];
    u16 speedos[HocClkSpeedo_EnumMax];
    u16 iddq[HocClkSpeedo_EnumMax];
    u16 waferX;
    u16 waferY;

    // Misc stuff
    GpuSchedulingMode gpuSchedulingMode;
    bool isSysDockInstalled;
    bool isSaltyNXInstalled;
    bool isUsingRetroSuper;
    u8 maxDisplayFreq;
    u8 dramID;
    bool isDram8GB;

    // FPS / Resolution
    u8 fps;
    u16 resolutionHeight;
} HocClkContext;

typedef struct
{
    union {
        uint32_t mhz[+HocClkProfile_EnumMax * +HocClkModule_EnumMax];
        uint32_t mhzMap[+HocClkProfile_EnumMax][+HocClkModule_EnumMax];
    };
} HocClkTitleProfileList;

#define HOCCLK_FREQ_LIST_MAX 32

#define GLOBAL_PROFILE_ID 0xA111111111111111