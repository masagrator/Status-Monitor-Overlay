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
    HocClkSocType_Erista = 0, // T210, found in Icosa and Copper
    HocClkSocType_Mariko,     // T214/T210B01, found in Hoag, Iowa, Calcio and Aula
//  HocClkSocType_Drake,      // T239, found in Switch 2. Maybe someday...
    HocClkSocType_EnumMax
} HocClkSocType;

typedef enum
{
    HocClkConsoleType_Icosa = 0, // V1
    HocClkConsoleType_Iowa,      // V2
    HocClkConsoleType_Hoag,      // Lite
    HocClkConsoleType_Aula,      // OLED
    HocClkConsoleType_EnumMax,
} HocClkConsoleType;

typedef enum {
    HocClkVoltage_SOC = 0, // VDD_SOC rail. 
    HocClkVoltage_EMCVDD2, // DRAM VDD2 rail
    HocClkVoltage_CPU,     // CPU rail
    HocClkVoltage_GPU,     // GPU rail
    HocClkVoltage_EMCVDDQ, // DRAM VDDQ rail
    HocClkVoltage_Display, // Display rail
    HocClkVoltage_Battery, // Battery voltage
    HocClkVoltage_EnumMax,
} HocClkVoltage;

typedef enum
{
    HocClkProfile_Handheld = 0,
    HocClkProfile_HandheldCharging, // Not a real profile, just a marker
    HocClkProfile_HandheldChargingUSB,
    HocClkProfile_HandheldChargingOfficial,
    HocClkProfile_Docked, // Not shown on Lites
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
    HocClkThermalSensor_SOC = 0, // SoC temperature in millicelcius
    HocClkThermalSensor_PCB,     // PCB temperature in millicelcius
    HocClkThermalSensor_Skin,    // "Skin" temperature in millicelcius
    HocClkThermalSensor_Battery, // Battery temperature in millicelcius
    HocClkThermalSensor_PMIC, // Always return 50.0C, as thats the only reasonable value the PMIC sensor can generate
    HocClkThermalSensor_CPU, // CPU temperature in millicelcius
    HocClkThermalSensor_GPU, // GPU temperature in millicelcius
    HocClkThermalSensor_MEM, // MEM temperature in millicelcius. Returns the PLLX sensor value on Mariko
    HocClkThermalSensor_PLLX, // PLLX temperature in millicelcius
    HocClkThermalSensor_AO, // AOTAG
    HocClkThermalSensor_BQ24193, // BQ24193 temperature. Refer to BQ24193Temp for returned values
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
    HocClkPartLoad_BAT, // Battery raw charge percentage
    HocClkPartLoad_FAN,
    HocClkPartLoad_RamBWAll,
    HocClkPartLoad_RamBWCpu,
    HocClkPartLoad_RamBWGpu,
    HocClkPartLoad_RamBWPeak, // Maximum possible RAM bandwidth
    HocClkPartLoad_EnumMax
} HocClkPartLoad;

typedef enum {
    HocClkSpeedo_CPU = 0,
    HocClkSpeedo_GPU,
    HocClkSpeedo_SOC,
    HocClkSpeedo_EnumMax,
} HocClkSpeedo;

typedef enum {
    GPUUVLevel_HiOPT = 0,
    GPUUVLevel_HiOPT15,
    GPUUVLevel_HighUV,
    GPUUVLevel_EnumMax,
} GPUUndervoltLevel;

enum {
    DVFSMode_Disabled = 0,
    DVFSMode_Hijack, // PCV hijack dvfs
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
    MemoryFrequencyMeasurementMode_PLL = 0,
    MemoryFrequencyMeasurementMode_Actmon,
    MemoryFrequencyMeasurementMode_EnumMax,
} MemoryFrequencyMeasurementMode;

typedef enum {
    RamDisplayUnit_MHz = 0,
    RamDisplayUnit_MTs,
    RamDisplayUnit_MHzMTs,
    RamDisplayUnit_EnumMax,
} RamDisplayUnit;
typedef enum {
    BQ24193Temp_Normal = 0,
    BQ24193Temp_Warm,
    BQ24193Temp_Hot,
    BQ24193Temp_Overheat,
    BQ24193Temp_EnumMax
} BQ24193Temp;
typedef enum AulaColorMode {
    AulaDisplayColorMode_DoNotOverride = 0xFF,
    AulaDisplayColorMode_Saturated = 0x0,
    AulaDisplayColorMode_Washed = 0x45,
    AulaDisplayColorMode_Basic = 0x03, // Default
    AulaDisplayColorMode_PowerReset = 0x20, // Reset to on power on
    AulaDisplayColorMode_Natural = 0x23,
    AulaDisplayColorMode_Vivid = 0x65,
    AulaDisplayColorMode_Night0 = 0x43,
    AulaDisplayColorMode_Night1 = 0x15,
    AulaDisplayColorMode_Night2 = 0x35,
    AulaDisplayColorMode_Night3 = 0x75,
} AulaColorMode;

// typedef enum {
// 	PANEL_JDI_XXX062M     = 0x10,
// 	PANEL_JDI_LAM062M109A = 0x0910, // SI.
// 	PANEL_JDI_LPM062M326A = 0x2610, // LTPS.
// 	PANEL_INL_P062CCA_AZ1 = 0x0F20,
// 	PANEL_AUO_A062TAN01   = 0x0F30,
// 	PANEL_INL_2J055IA_27A = 0x1020,
// 	PANEL_AUO_A055TAN01   = 0x1030,
// 	PANEL_SHP_LQ055T1SW10 = 0x1040,
// 	PANEL_SAM_AMS699VC01  = 0x2050,
    
// 	PANEL_RR_SUPER5_OLED_V1  = 0x10E0,
// 	PANEL_RR_SUPER5_OLED_HD_V1  = 0x10E1,
// 	PANEL_RR_SUPER7_IPS_V1  = 0x0FE0,
// 	PANEL_RR_SUPER7_IPS_HD_V1  = 0x0FE1
// 	// Found on 6/2" clones. Unknown markings. Clone of AUO A062TAN01.
// 	// Quality seems JDI like. Has bad low backlight scaling. ID: [83] 94 [0F]. Sometimes reports [30] 94 [0F]. Both IDs have correct CRC16.
// 	PANEL_OEM_CLONE_6_2   = 0x0F83,
// 	// Found on 5.5" clones with AUO A055TAN02 (59.05A30.001) fake markings.
// 	PANEL_OEM_CLONE_5_5   = 0x00B3,
// 	// Found on 5.5" clones with AUO A055TAN02 (59.05A30.001) fake markings.
// 	PANEL_OEM_CLONE       = 0x0000
// 	//0x0F40 [40] 94 [0F], 5.5" clone
// } HocClkDisplayPanel;

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
        case HocClkThermalSensor_AO:
            return pretty ? "AO" : "ao";
        case HocClkThermalSensor_BQ24193:
            return pretty ? "BQ24193" : "bq24193";
        default:
            return "unknown";
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
            return "unknown";
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
            return "unknown";
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
            return pretty ? "VDD2" : "vdd2";
        case HocClkVoltage_EMCVDDQ:
            return pretty ? "VDDQ" : "vddq";
        case HocClkVoltage_SOC:
            return pretty ? "SOC" : "soc";
        case HocClkVoltage_Display:
            return pretty ? "Display" : "display";
        default:
            return "unknown";
    }
}

static inline const char* hocClkFormatConsoleType(HocClkConsoleType consoleType, bool pretty)
{
    switch(consoleType)
    {
        case HocClkConsoleType_Icosa:
            return pretty ? "Icosa (V1)" : "icosa";
        case HocClkConsoleType_Iowa:
            return pretty ? "Iowa (V2)" : "iowa";
        case HocClkConsoleType_Hoag:
            return pretty ? "Hoag (Lite)" : "hoag";
        case HocClkConsoleType_Aula:
            return pretty ? "Aula (OLED)" : "aula";
        default:
            return "unknown";
    }
}

// static inline const char* hocClkFormatPanel(HocClkDisplayPanel panel, bool pretty)
// {
//     switch(panel)
//     {
//         case PANEL_JDI_XXX062M:
//             return pretty ? "JDI XXX062M" : "jdi_xxx062m";
//         case PANEL_JDI_LAM062M109A:
//             return pretty ? "JDI LAM062M109A" : "jdi_lam062m109a";
//         case PANEL_JDI_LPM062M326A:
//             return pretty ? "JDI LPM062M326A" : "jdi_lpm062m326a";
//         case PANEL_INL_P062CCA_AZ1:
//             return pretty ? "INL P062CCA-AZ1" : "inl_p062cca_az1";
//         case PANEL_AUO_A062TAN01:
//             return pretty ? "AUO A062TAN01" : "auo_a062tan01";
//         case PANEL_INL_2J055IA_27A:
//             return pretty ? "INL 2J055IA-27A" : "inl_2j055ia_27a";
//         case PANEL_AUO_A055TAN01:
//             return pretty ? "AUO A055TAN01" : "auo_a055tan01";
//         case PANEL_SHP_LQ055T1SW10:
//             return pretty ? "SHP LQ055T1SW10" : "shp_lq055t1sw10";
//         case PANEL_SAM_AMS699VC01:
//             return pretty ? "SAM AMS699VC01" : "sam_ams699vc01";
//         case PANEL_RR_SUPER5_OLED_V1:
//             return pretty ? "SUPER5 OLED" : "rr_super5_oled_v1";
//         case PANEL_RR_SUPER5_OLED_HD_V1:
//             return pretty ? "SUPER5 OLED HD" : "rr_super5_oled_hd_v1";
//         case PANEL_RR_SUPER7_IPS_V1:
//             return pretty ? "SUPER7 IPS" : "rr_super7_ips_v1";
//         case PANEL_RR_SUPER7_IPS_HD_V1:
//             return pretty ? "RR Super7 IPS HD V1" : "rr_super7_ips_hd_v1";
//         case PANEL_OEM_CLONE_6_2:
//             return pretty ? "OEM Clone 6.2" : "oem_clone_6_2";
//         case PANEL_OEM_CLONE_5_5:
//             return pretty ? "OEM Clone 5.5" : "oem_clone_5_5";
//         case PANEL_OEM_CLONE:
//             return pretty ? "OEM Clone" : "oem_clone";
//         default:
//             return "unknown";
//     }
// }