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

typedef struct {

    /*
     * This "stable struct" must never be modified. It provides a fixed memory layout so external clients can safely read the expected fields even
     * if HocClkContext changes in newer versions and the client is not recompiled.
     */
    struct {
        #define HocClkModuleStable_EnumMax 5
        #define HocClkThermalSensorStable_EnumMax 11
        #define HocClkPowerSensorStable_EnumMax 2
        #define HocClkPartLoadStable_EnumMax 10
        #define HocClkVoltageStable_EnumMax 7

        u32 freqs[HocClkModuleStable_EnumMax];
        u32 realFreqs[HocClkModuleStable_EnumMax];
        u32 overrideFreqs[HocClkModuleStable_EnumMax];
        s32 temps[HocClkThermalSensorStable_EnumMax];
        s32 power[HocClkPowerSensorStable_EnumMax];
        u32 partLoad[HocClkPartLoadStable_EnumMax];
        u32 voltages[HocClkVoltageStable_EnumMax];
    } stable;

    uint64_t applicationId;
    HocClkProfile profile;
    uint32_t freqs[HocClkModule_EnumMax];
    uint32_t realFreqs[HocClkModule_EnumMax];
    uint32_t overrideFreqs[HocClkModule_EnumMax];
    int32_t temps[HocClkThermalSensor_EnumMax];
    int32_t power[HocClkPowerSensor_EnumMax];
    uint32_t partLoad[HocClkPartLoad_EnumMax];
    uint32_t voltages[HocClkVoltage_EnumMax];
    u16 speedos[HocClkSpeedo_EnumMax];
    u16 iddq[HocClkSpeedo_EnumMax];
    s16 waferX;
    s16 waferY;

    // Misc stuff
    GpuSchedulingMode gpuSchedulingMode;
    bool isSysDockInstalled;
    bool isSaltyNXInstalled;
    bool isUsingRetroSuper;
    u8 maxDisplayFreq;
    u8 dramID;
    bool isDram8GB;
    HocClkConsoleType consoleType;

    // FPS / Resolution
    u8 fps;
    u16 resolutionHeight;
    u8 custRev;
    u16 kipVersion;

    // Reserved for future use
    u8 reserved[0x35B];
} HocClkContext;

typedef struct
{
    union {
        uint32_t mhz[+HocClkProfile_EnumMax * +HocClkModule_EnumMax];
        uint32_t mhzMap[+HocClkProfile_EnumMax][+HocClkModule_EnumMax];
    };
} HocClkTitleProfileList;

#define HOCCLK_FREQ_LIST_MAX 48

#define HOCCLK_GLOBAL_PROFILE_TID 0xA111111111111111

static_assert(sizeof(HocClkContext) == 0x500);