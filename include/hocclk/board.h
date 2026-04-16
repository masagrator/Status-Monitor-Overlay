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
#include <stddef.h>
#include <stdbool.h>
#include <switch/types.h>
typedef enum
{
    HocClkSocType_Erista = 0,
    HocClkSocType_Mariko,
    HocClkSocType_EnumMax
} HocClkSocType;

typedef enum
{
    HocClkConsoleType_Icosa = 0,
    HocClkConsoleType_Copper,
    HocClkConsoleType_Hoag,
    HocClkConsoleType_Iowa,
    HocClkConsoleType_Calcio,
    HocClkConsoleType_Aula,
    HocClkConsoleType_EnumMax,
} HocClkConsoleType;

typedef enum {
    HocClkVoltage_SOC = 0,
    HocClkVoltage_EMCVDD2,
    HocClkVoltage_CPU,
    HocClkVoltage_GPU,
    HocClkVoltage_EMCVDDQ,
    HocClkVoltage_Display,
    HocClkVoltage_Battery,
    HocClkVoltage_EnumMax,
} HocClkVoltage;

typedef enum
{
    HocClkProfile_Handheld = 0,
    HocClkProfile_HandheldCharging,
    HocClkProfile_HandheldChargingUSB,
    HocClkProfile_HandheldChargingOfficial,
    HocClkProfile_Docked,
    HocClkProfile_EnumMax
} HocClkProfile;

typedef enum
{
    HocClkModule_CPU = 0,
    HocClkModule_GPU,
    HocClkModule_MEM,
    HocClkModule_Governor,
    HocClkModule_Display,
    HocClkModule_EnumMax,
} HocClkModule;

typedef enum
{
    HocClkThermalSensor_SOC = 0,
    HocClkThermalSensor_PCB,
    HocClkThermalSensor_Skin,
    HocClkThermalSensor_Battery,
    HocClkThermalSensor_PMIC,
    HocClkThermalSensor_CPU,
    HocClkThermalSensor_GPU,
    HocClkThermalSensor_MEM,
    HocClkThermalSensor_PLLX,
    HocClkThermalSensor_EnumMax
} HocClkThermalSensor;

typedef enum
{
    HocClkPowerSensor_Now = 0,
    HocClkPowerSensor_Avg,
    HocClkPowerSensor_EnumMax
} HocClkPowerSensor;

typedef enum
{
    HocClkPartLoad_EMC = 0,
    HocClkPartLoad_EMCCpu,
    HocClkPartLoad_GPU,
    HocClkPartLoad_CPUMax,
    HocClkPartLoad_BAT,
    HocClkPartLoad_FAN,
    HocClkPartLoad_RamBWAll,
    HocClkPartLoad_RamBWCpu,
    HocClkPartLoad_RamBWGpu,
    HocClkPartLoad_RamBWPeak,
    HocClkPartLoad_EnumMax
} HocClkPartLoad;

typedef enum {
    HocClkSpeedo_CPU = 0,
    HocClkSpeedo_GPU,
    HocClkSpeedo_SOC,
    HocClkSpeedo_EnumMax,
} HocClkSpeedo;

typedef enum {
    GPUUVLevel_NoUV = 0,
    GPUUVLevel_SLT,
    GPUUVLevel_HiOPT,
    GPUUVLevel_EnumMax,
} GPUUndervoltLevel;

enum {
    DVFSMode_Disabled = 0,
    DVFSMode_Hijack,
    // DVFSMode_OfficialService,
    // DVFSMode_Hack,
    DVFSMode_EnumMax,
};

typedef enum {
    GpuSchedulingMode_DoNotOverride = 0,
    GpuSchedulingMode_Enabled,
    GpuSchedulingMode_Disabled,
    GpuSchedulingMode_EnumMax,
} GpuSchedulingMode;

typedef enum {
    GpuSchedulingOverrideMethod_Ini = 0,
    GpuSchedulingOverrideMethod_NvService,
    GpuSchedulingOverrideMethod_EnumMax,
} GpuSchedulingOverrideMethod;
typedef enum {
    ComponentGovernor_DoNotOverride = 0,
    ComponentGovernor_Disabled      = 1,
    ComponentGovernor_Enabled       = 2,
    ComponentGovernor_EnumMax,
} ComponentGovernorState;
typedef enum {
    RamDisplayMode_VDD2 = 0,
    RamDisplayMode_VDDQ,
    RamDisplayMode_EnumMax,
} RamDisplayMode;

typedef enum {
    MemoryFrequencyMeasurementMode_Actmon = 0,
    MemoryFrequencyMeasurementMode_PLL,
    MemoryFrequencyMeasurementMode_EnumMax,
} MemoryFrequencyMeasurementMode;

typedef enum {
    RamDisplayUnit_MHz = 0,
    RamDisplayUnit_MTs,
    RamDisplayUnit_MHzMTs,
    RamDisplayUnit_EnumMax,
} RamDisplayUnit;

#define HOCCLK_ENUM_VALID(n, v) ((v) < n##_EnumMax)

// Packed u32
// Bits 0-7 - CPU
// Bits 8-15 - GPU
// Bits 16-23 - VRR
// Bits 24-32 - unused

inline u32 GovernorStatePack(u8 cpu, u8 gpu, u8 vrr) {
    return (u32)cpu | ((u32)gpu << 8) | ((u32)vrr << 16);
}
inline u8 GovernorStateCpu(u32 p) {
    return (u8)(p         & 0xFF);
}
inline u8 GovernorStateGpu(u32 p) {
    return (u8)((p >>  8) & 0xFF);
}
inline u8 GovernorStateVrr(u32 p) {
    return (u8)((p >> 16) & 0xFF);
}

static inline const char* hocclkFormatModule(HocClkModule module, bool pretty)
{
    switch(module)
    {
        case HocClkModule_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkModule_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkModule_MEM:
            return pretty ? "Memory" : "mem";
        case HocClkModule_Display:
            return pretty ? "Display" : "display";
        case HocClkModule_Governor:
            return pretty ? "Governor" : "governor";
        default:
            return "null";
    }
}

static inline const char* hocclkFormatThermalSensor(HocClkThermalSensor thermSensor, bool pretty)
{
    switch(thermSensor) {
        case HocClkThermalSensor_SOC:
            return pretty ? "SOC" : "soc";
        case HocClkThermalSensor_PCB:
            return pretty ? "PCB" : "pcb";
        case HocClkThermalSensor_Skin:
            return pretty ? "Skin" : "skin";
        case HocClkThermalSensor_Battery:
            return pretty ? "BAT" : "battery";
        case HocClkThermalSensor_PMIC:
            return pretty ? "PMIC" : "pmic";
        case HocClkThermalSensor_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkThermalSensor_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkThermalSensor_MEM:
            return pretty ? "MEM" : "mem";
        case HocClkThermalSensor_PLLX:
            return pretty ? "PLLX" : "pllx";
        default:
            return NULL;
    }
}

static inline const char* hocclkFormatPowerSensor(HocClkPowerSensor powSensor, bool pretty)
{
    switch(powSensor)
    {
        case HocClkPowerSensor_Now:
            return pretty ? "Now" : "now";
        case HocClkPowerSensor_Avg:
            return pretty ? "Avg" : "avg";
        default:
            return NULL;
    }
}

static inline const char* hocclkFormatProfile(HocClkProfile profile, bool pretty)
{
    switch(profile)
    {
        case HocClkProfile_Docked:
            return pretty ? "Docked" : "docked";
        case HocClkProfile_Handheld:
            return pretty ? "Handheld" : "handheld";
        case HocClkProfile_HandheldCharging:
            return pretty ? "Charging" : "handheld_charging";
        case HocClkProfile_HandheldChargingUSB:
            return pretty ? "USB Charger" : "handheld_charging_usb";
        case HocClkProfile_HandheldChargingOfficial:
            return pretty ? "PD Charger" : "handheld_charging_official";
        default:
            return NULL;
    }
}


static inline const char* hocClkFormatVoltage(HocClkVoltage voltage, bool pretty)
{
    switch(voltage)
    {
        case HocClkVoltage_CPU:
            return pretty ? "CPU" : "cpu";
        case HocClkVoltage_GPU:
            return pretty ? "GPU" : "gpu";
        case HocClkVoltage_EMCVDD2:
            return pretty ? "VDD2" : "emcvdd2";
        case HocClkVoltage_EMCVDDQ:
            return pretty ? "VDDQ" : "vddq";
        case HocClkVoltage_SOC:
            return pretty ? "SOC" : "soc";
        case HocClkVoltage_Display:
            return pretty ? "Display" : "display";
        default:
            return NULL;
    }
}
