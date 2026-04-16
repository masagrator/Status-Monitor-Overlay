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
#include "board.h"
typedef enum {
    HocClkConfigValue_PollingIntervalMs = 0,
    HocClkConfigValue_TempLogIntervalMs,
    HocClkConfigValue_FreqLogIntervalMs,
    HocClkConfigValue_PowerLogIntervalMs,
    HocClkConfigValue_CsvWriteIntervalMs,

    HocClkConfigValue_UncappedClocks,
    HocClkConfigValue_OverwriteBoostMode,

    HocClkConfigValue_EristaMaxCpuClock,
    HocClkConfigValue_MarikoMaxCpuClock,

    HocClkConfigValue_ThermalThrottle,
    HocClkConfigValue_ThermalThrottleThreshold,

    HocClkConfigValue_HandheldTDP,
    HocClkConfigValue_HandheldTDPLimit,

    HocClkConfigValue_LiteTDPLimit,

    HocClkConfigValue_BatteryChargeCurrent,

    HocClkConfigValue_OverwriteRefreshRate,
    HocClkConfigValue_MaxDisplayClockH,

    HocClkConfigValue_DVFSMode,
    HocClkConfigValue_DVFSOffset,
    HocClkConfigValue_LiveCpuUv,
    HocClkConfigValue_EnableExperimentalSettings,

    HocClkConfigValue_GPUScheduling,
    HocClkConfigValue_GPUSchedulingMethod,

    HocClkConfigValue_RAMVoltDisplayMode,
    HocClkConfigValue_CpuGovernorMinimumFreq,
    HocClkConfigValue_DisplayVoltage,

    HocClkConfigValue_MemoryFrequencyMeasurementMode,
    HocClkConfigValue_RamDisplayUnit,

    KipConfigValue_custRev,
    // KipConfigValue_mtcConf,
    KipConfigValue_hpMode,

    KipConfigValue_commonEmcMemVolt,
    KipConfigValue_eristaEmcMaxClock,
    KipConfigValue_eristaEmcMaxClock1,
    KipConfigValue_eristaEmcMaxClock2,
    KipConfigValue_marikoEmcMaxClock,
    KipConfigValue_marikoEmcVddqVolt,
    KipConfigValue_emcDvbShift,

    KipConfigValue_t1_tRCD,
    KipConfigValue_t2_tRP,
    KipConfigValue_t3_tRAS,
    KipConfigValue_t4_tRRD,
    KipConfigValue_t5_tRFC,
    KipConfigValue_t6_tRTW,
    KipConfigValue_t7_tWTR,
    KipConfigValue_t8_tREFI,
    KipConfigValue_mem_burst_read_latency,
    KipConfigValue_mem_burst_write_latency,

    KipConfigValue_eristaCpuUV,
    KipConfigValue_eristaCpuVmin,
    KipConfigValue_eristaCpuMaxVolt,
    KipConfigValue_eristaCpuUnlock,

    KipConfigValue_marikoCpuUVLow,
    KipConfigValue_marikoCpuUVHigh,
    KipConfigValue_tableConf,
    KipConfigValue_marikoCpuLowVmin,
    KipConfigValue_marikoCpuHighVmin,
    KipConfigValue_marikoCpuMaxVolt,
    KipConfigValue_marikoCpuMaxClock,
    KipConfigValue_eristaCpuBoostClock,
    KipConfigValue_marikoCpuBoostClock,

    KipConfigValue_eristaGpuUV,
    KipConfigValue_eristaGpuVmin,

    KipConfigValue_marikoGpuUV,
    KipConfigValue_marikoGpuVmin,
    KipConfigValue_marikoGpuVmax,

    KipConfigValue_commonGpuVoltOffset,
    KipConfigValue_gpuSpeedo,

    KipConfigValue_g_volt_76800,
    KipConfigValue_g_volt_153600,
    KipConfigValue_g_volt_230400,
    KipConfigValue_g_volt_307200,
    KipConfigValue_g_volt_384000,
    KipConfigValue_g_volt_460800,
    KipConfigValue_g_volt_537600,
    KipConfigValue_g_volt_614400,
    KipConfigValue_g_volt_691200,
    KipConfigValue_g_volt_768000,
    KipConfigValue_g_volt_844800,
    KipConfigValue_g_volt_921600,
    KipConfigValue_g_volt_998400,
    KipConfigValue_g_volt_1075200,
    KipConfigValue_g_volt_1152000,
    KipConfigValue_g_volt_1228800,
    KipConfigValue_g_volt_1267200,
    KipConfigValue_g_volt_1305600,
    KipConfigValue_g_volt_1344000,
    KipConfigValue_g_volt_1382400,
    KipConfigValue_g_volt_1420800,
    KipConfigValue_g_volt_1459200,
    KipConfigValue_g_volt_1497600,
    KipConfigValue_g_volt_1536000,

    KipConfigValue_g_volt_e_76800,
    KipConfigValue_g_volt_e_115200,
    KipConfigValue_g_volt_e_153600,
    KipConfigValue_g_volt_e_192000,
    KipConfigValue_g_volt_e_230400,
    KipConfigValue_g_volt_e_268800,
    KipConfigValue_g_volt_e_307200,
    KipConfigValue_g_volt_e_345600,
    KipConfigValue_g_volt_e_384000,
    KipConfigValue_g_volt_e_422400,
    KipConfigValue_g_volt_e_460800,
    KipConfigValue_g_volt_e_499200,
    KipConfigValue_g_volt_e_537600,
    KipConfigValue_g_volt_e_576000,
    KipConfigValue_g_volt_e_614400,
    KipConfigValue_g_volt_e_652800,
    KipConfigValue_g_volt_e_691200,
    KipConfigValue_g_volt_e_729600,
    KipConfigValue_g_volt_e_768000,
    KipConfigValue_g_volt_e_806400,
    KipConfigValue_g_volt_e_844800,
    KipConfigValue_g_volt_e_883200,
    KipConfigValue_g_volt_e_921600,
    KipConfigValue_g_volt_e_960000,
    KipConfigValue_g_volt_e_998400,
    KipConfigValue_g_volt_e_1036800,
    KipConfigValue_g_volt_e_1075200,

    KipConfigValue_t6_tRTW_fine_tune,
    KipConfigValue_t7_tWTR_fine_tune,

    KipCrc32,
    HocClkConfigValue_IsFirstLoad,
    HocClkConfigValue_EnumMax,
} HocClkConfigValue;

typedef struct {
    uint64_t values[HocClkConfigValue_EnumMax];
} HocClkConfigValueList;

static inline const char* hocclkFormatConfigValue(HocClkConfigValue val, bool pretty)
{
    switch(val)
    {
        case HocClkConfigValue_PollingIntervalMs:
            return pretty ? "Polling Interval (ms)" : "poll_interval_ms";
        case HocClkConfigValue_TempLogIntervalMs:
            return pretty ? "Temperature logging interval (ms)" : "temp_log_interval_ms";
        case HocClkConfigValue_FreqLogIntervalMs:
            return pretty ? "Frequency logging interval (ms)" : "freq_log_interval_ms";
        case HocClkConfigValue_PowerLogIntervalMs:
            return pretty ? "Power logging interval (ms)" : "power_log_interval_ms";
        case HocClkConfigValue_CsvWriteIntervalMs:
            return pretty ? "CSV write interval (ms)" : "csv_write_interval_ms";

        case HocClkConfigValue_UncappedClocks:
            return pretty ? "Uncapped Clocks" : "uncapped_clocks";
        case HocClkConfigValue_OverwriteBoostMode:
            return pretty ? "Overwrite Boost Mode" : "ow_boost";

        case HocClkConfigValue_EristaMaxCpuClock:
            return pretty ? "CPU Max Clock" : "cpu_max_e";

        case HocClkConfigValue_MarikoMaxCpuClock:
            return pretty ? "CPU Max Display Clock" : "cpu_max_m";

        case HocClkConfigValue_ThermalThrottle:
            return pretty ? "Thermal Throttle" : "thermal_throttle";

        case HocClkConfigValue_ThermalThrottleThreshold:
            return pretty ? "Thermal Throttle Threshold" : "thermal_throttle_threshold";

        case HocClkConfigValue_HandheldTDP:
            return pretty ? "Handheld TDP" : "handheld_tdp";

        case HocClkConfigValue_HandheldTDPLimit:
            return pretty ? "Handheld TDP Limit" : "tdp_limit";

        case HocClkConfigValue_LiteTDPLimit:
            return pretty ? "Handheld TDP Limit" : "tdp_limit_l";

        case HocClkConfigValue_BatteryChargeCurrent:
            return pretty ? "Battery Charge Current" : "bat_charge_current";

        case HocClkConfigValue_OverwriteRefreshRate:
            return pretty ? "Display Refresh Rate Changing" : "drr_changing";

        case HocClkConfigValue_MaxDisplayClockH:
            return pretty ? "Max Display Clock (Handheld)" : "drr_max_clock";

        case HocClkConfigValue_DVFSMode:
            return pretty ? "DVFS Mode" : "dvfs_mode";

        case HocClkConfigValue_DVFSOffset:
            return pretty ? "DVFS Offset" : "dvfs_offset";

        case HocClkConfigValue_GPUScheduling:
            return pretty ? "GPU Scheduling" : "gpu_scheduling";

        case HocClkConfigValue_GPUSchedulingMethod:
            return pretty ? "GPU Scheduling Method" : "gpu_sched_method";

        case HocClkConfigValue_LiveCpuUv:
            return pretty ? "Live CPU Undervolt" : "live_cpu_uv";

        case HocClkConfigValue_EnableExperimentalSettings:
            return pretty ? "Enable Experimental Settings" : "enable_experimental_settings";

        case HocClkConfigValue_RAMVoltDisplayMode:
            return pretty ? "RAM Voltage / Usage Display Mode" : "ram_volt_usage_display_mode";
        case HocClkConfigValue_CpuGovernorMinimumFreq:
            return pretty ? "CPU Governor Minimum Frequency" : "cpu_gov_min_freq";

        case HocClkConfigValue_DisplayVoltage:
            return pretty ? "Display Voltage" : "display_voltage";

        case HocClkConfigValue_MemoryFrequencyMeasurementMode:
            return pretty ? "RAM Frequency Measurement Mode" : "mem_freq_measurement_mode";

        case HocClkConfigValue_RamDisplayUnit:
            return pretty ? "RAM Frequency Display Unit" : "RAM_display_unit";

        // KIP config values
        case KipConfigValue_custRev:
            return pretty ? "Custom Revision" : "kip_cust_rev";
        // case KipConfigValue_mtcConf:
        //     return pretty ? "MTC Config" : "kip_mtc_conf";
        case KipConfigValue_hpMode:
            return pretty ? "HP Mode" : "kip_hp_mode";

        // EMC
        case KipConfigValue_commonEmcMemVolt:
            return pretty ? "Common EMC/MEM Voltage" : "common_emc_mem_volt";
        case KipConfigValue_eristaEmcMaxClock:
            return pretty ? "Erista EMC Max Clock 1" : "erista_emc_max_clock";
        case KipConfigValue_eristaEmcMaxClock1:
            return pretty ? "Erista EMC Max Clock 2" : "erista_emc_max_clock1";
        case KipConfigValue_eristaEmcMaxClock2:
            return pretty ? "Erista EMC Max Clock 3" : "erista_emc_max_clock2";
        case KipConfigValue_marikoEmcMaxClock:
            return pretty ? "Mariko EMC Max Clock" : "mariko_emc_max_clock";
        case KipConfigValue_marikoEmcVddqVolt:
            return pretty ? "Mariko EMC VDDQ Voltage" : "mariko_emc_vddq_volt";
        case KipConfigValue_emcDvbShift:
            return pretty ? "EMC DVB Shift" : "emc_dvb_shift";

        // Memory timings
        case KipConfigValue_t1_tRCD:
            return pretty ? "t1 - tRCD" : "t1_trcd";
        case KipConfigValue_t2_tRP:
            return pretty ? "t2 - tRP" : "t2_trp";
        case KipConfigValue_t3_tRAS:
            return pretty ? "t3 - tRAS" : "t3_tras";
        case KipConfigValue_t4_tRRD:
            return pretty ? "t4 - tRRD" : "t4_trrd";
        case KipConfigValue_t5_tRFC:
            return pretty ? "t5 - tRFC" : "t5_trfc";
        case KipConfigValue_t6_tRTW:
            return pretty ? "t6 - tRTW" : "t6_trtw";
        case KipConfigValue_t7_tWTR:
            return pretty ? "t7 - tWTR" : "t7_twtr";
        case KipConfigValue_t8_tREFI:
            return pretty ? "t8 - tREFI" : "t8_trefi";
        case KipConfigValue_mem_burst_read_latency:
            return pretty ? "Memory Burst Read Latency" : "mem_burst_read_latency";
        case KipConfigValue_mem_burst_write_latency:
            return pretty ? "Memory Burst Write Latency" : "mem_burst_write_latency";

        // CPU – Erista
        case KipConfigValue_eristaCpuUV:
            return pretty ? "Erista CPU Undervolt" : "erista_cpu_uv";
        case KipConfigValue_eristaCpuVmin:
            return pretty ? "Erista CPU vMin" : "erista_cpu_vmin";
        case KipConfigValue_eristaCpuMaxVolt:
            return pretty ? "Erista CPU Max Voltage" : "erista_cpu_max_volt";
        case KipConfigValue_eristaCpuUnlock:
            return pretty ? "Erista CPU Unlock" : "erista_cpu_unlock";

        // CPU – Mariko
        case KipConfigValue_marikoCpuUVLow:
            return pretty ? "Mariko CPU Undervolt (Low)" : "mariko_cpu_uv_low";
        case KipConfigValue_marikoCpuUVHigh:
            return pretty ? "Mariko CPU Undervolt (High)" : "mariko_cpu_uv_high";
        case KipConfigValue_tableConf:
            return pretty ? "Table Config" : "kip_table_conf";
        case KipConfigValue_marikoCpuLowVmin:
            return pretty ? "Mariko CPU Low Vmin" : "mariko_cpu_low_vmin";
        case KipConfigValue_marikoCpuHighVmin:
            return pretty ? "Mariko CPU High Vmin" : "mariko_cpu_high_vmin";
        case KipConfigValue_marikoCpuMaxVolt:
            return pretty ? "Mariko CPU Max Voltage" : "mariko_cpu_max_volt";

        case KipConfigValue_eristaCpuBoostClock:
            return pretty ? "Erista CPU Boost Clock" : "erista_cpu_boost_clock";
        case KipConfigValue_marikoCpuBoostClock:
            return pretty ? "Mariko CPU Boost Clock" : "mariko_cpu_boost_clock";

        case KipConfigValue_marikoCpuMaxClock:
            return pretty ? "Mariko CPU Max Clock" : "mariko_cpu_max_clock";

        // GPU – Erista
        case KipConfigValue_eristaGpuUV:
            return pretty ? "Erista GPU Undervolt" : "erista_gpu_uv";
        case KipConfigValue_eristaGpuVmin:
            return pretty ? "Erista GPU Vmin" : "erista_gpu_vmin";

        // GPU – Mariko
        case KipConfigValue_marikoGpuUV:
            return pretty ? "Mariko GPU Undervolt" : "mariko_gpu_uv";
        case KipConfigValue_marikoGpuVmin:
            return pretty ? "Mariko GPU Vmin" : "mariko_gpu_vmin";
        case KipConfigValue_marikoGpuVmax:
            return pretty ? "Mariko GPU Vmax" : "mariko_gpu_vmax";

        case KipConfigValue_commonGpuVoltOffset:
            return pretty ? "Common GPU Voltage Offset" : "common_gpu_volt_offset";
        case KipConfigValue_gpuSpeedo:
            return pretty ? "GPU Speedo" : "gpu_speedo";

        // Mariko GPU voltages (24)
        case KipConfigValue_g_volt_76800: return pretty ? "Mariko GPU Volt 76 MHz" : "g_volt_76800";
        case KipConfigValue_g_volt_153600: return pretty ? "Mariko GPU Volt 153 MHz" : "g_volt_153600";
        case KipConfigValue_g_volt_230400: return pretty ? "Mariko GPU Volt 230 MHz" : "g_volt_230400";
        case KipConfigValue_g_volt_307200: return pretty ? "Mariko GPU Volt 307 MHz" : "g_volt_307200";
        case KipConfigValue_g_volt_384000: return pretty ? "Mariko GPU Volt 384 MHz" : "g_volt_384000";
        case KipConfigValue_g_volt_460800: return pretty ? "Mariko GPU Volt 460 MHz" : "g_volt_460800";
        case KipConfigValue_g_volt_537600: return pretty ? "Mariko GPU Volt 537 MHz" : "g_volt_537600";
        case KipConfigValue_g_volt_614400: return pretty ? "Mariko GPU Volt 614 MHz" : "g_volt_614400";
        case KipConfigValue_g_volt_691200: return pretty ? "Mariko GPU Volt 691 MHz" : "g_volt_691200";
        case KipConfigValue_g_volt_768000: return pretty ? "Mariko GPU Volt 768 MHz" : "g_volt_768000";
        case KipConfigValue_g_volt_844800: return pretty ? "Mariko GPU Volt 844 MHz" : "g_volt_844800";
        case KipConfigValue_g_volt_921600: return pretty ? "Mariko GPU Volt 921 MHz" : "g_volt_921600";
        case KipConfigValue_g_volt_998400: return pretty ? "Mariko GPU Volt 998 MHz" : "g_volt_998400";
        case KipConfigValue_g_volt_1075200: return pretty ? "Mariko GPU Volt 1075 MHz" : "g_volt_1075200";
        case KipConfigValue_g_volt_1152000: return pretty ? "Mariko GPU Volt 1152 MHz" : "g_volt_1152000";
        case KipConfigValue_g_volt_1228800: return pretty ? "Mariko GPU Volt 1228 MHz" : "g_volt_1228800";
        case KipConfigValue_g_volt_1267200: return pretty ? "Mariko GPU Volt 1267 MHz" : "g_volt_1267200";
        case KipConfigValue_g_volt_1305600: return pretty ? "Mariko GPU Volt 1305 MHz" : "g_volt_1305600";
        case KipConfigValue_g_volt_1344000: return pretty ? "Mariko GPU Volt 1344 MHz" : "g_volt_1344000";
        case KipConfigValue_g_volt_1382400: return pretty ? "Mariko GPU Volt 1382 MHz" : "g_volt_1382400";
        case KipConfigValue_g_volt_1420800: return pretty ? "Mariko GPU Volt 1420 MHz" : "g_volt_1420800";
        case KipConfigValue_g_volt_1459200: return pretty ? "Mariko GPU Volt 1459 MHz" : "g_volt_1459200";
        case KipConfigValue_g_volt_1497600: return pretty ? "Mariko GPU Volt 1497 MHz" : "g_volt_1497600";
        case KipConfigValue_g_volt_1536000: return pretty ? "Mariko GPU Volt 1536 MHz" : "g_volt_1536000";

        // Erista GPU voltages (27)
        case KipConfigValue_g_volt_e_76800: return pretty ? "Erista GPU Volt 76 MHz" : "g_volt_e_76800";
        case KipConfigValue_g_volt_e_115200: return pretty ? "Erista GPU Volt 115 MHz" : "g_volt_e_115200";
        case KipConfigValue_g_volt_e_153600: return pretty ? "Erista GPU Volt 153 MHz" : "g_volt_e_153600";
        case KipConfigValue_g_volt_e_192000: return pretty ? "Erista GPU Volt 192 MHz" : "g_volt_e_192000";
        case KipConfigValue_g_volt_e_230400: return pretty ? "Erista GPU Volt 230 MHz" : "g_volt_e_230400";
        case KipConfigValue_g_volt_e_268800: return pretty ? "Erista GPU Volt 268 MHz" : "g_volt_e_268800";
        case KipConfigValue_g_volt_e_307200: return pretty ? "Erista GPU Volt 307 MHz" : "g_volt_e_307200";
        case KipConfigValue_g_volt_e_345600: return pretty ? "Erista GPU Volt 345 MHz" : "g_volt_e_345600";
        case KipConfigValue_g_volt_e_384000: return pretty ? "Erista GPU Volt 384 MHz" : "g_volt_e_384000";
        case KipConfigValue_g_volt_e_422400: return pretty ? "Erista GPU Volt 422 MHz" : "g_volt_e_422400";
        case KipConfigValue_g_volt_e_460800: return pretty ? "Erista GPU Volt 460 MHz" : "g_volt_e_460800";
        case KipConfigValue_g_volt_e_499200: return pretty ? "Erista GPU Volt 499 MHz" : "g_volt_e_499200";
        case KipConfigValue_g_volt_e_537600: return pretty ? "Erista GPU Volt 537 MHz" : "g_volt_e_537600";
        case KipConfigValue_g_volt_e_576000: return pretty ? "Erista GPU Volt 576 MHz" : "g_volt_e_576000";
        case KipConfigValue_g_volt_e_614400: return pretty ? "Erista GPU Volt 614 MHz" : "g_volt_e_614400";
        case KipConfigValue_g_volt_e_652800: return pretty ? "Erista GPU Volt 652 MHz" : "g_volt_e_652800";
        case KipConfigValue_g_volt_e_691200: return pretty ? "Erista GPU Volt 691 MHz" : "g_volt_e_691200";
        case KipConfigValue_g_volt_e_729600: return pretty ? "Erista GPU Volt 729 MHz" : "g_volt_e_729600";
        case KipConfigValue_g_volt_e_768000: return pretty ? "Erista GPU Volt 768 MHz" : "g_volt_e_768000";
        case KipConfigValue_g_volt_e_806400: return pretty ? "Erista GPU Volt 806 MHz" : "g_volt_e_806400";
        case KipConfigValue_g_volt_e_844800: return pretty ? "Erista GPU Volt 844 MHz" : "g_volt_e_844800";
        case KipConfigValue_g_volt_e_883200: return pretty ? "Erista GPU Volt 883 MHz" : "g_volt_e_883200";
        case KipConfigValue_g_volt_e_921600: return pretty ? "Erista GPU Volt 921 MHz" : "g_volt_e_921600";
        case KipConfigValue_g_volt_e_960000: return pretty ? "Erista GPU Volt 960 MHz" : "g_volt_e_960000";
        case KipConfigValue_g_volt_e_998400: return pretty ? "Erista GPU Volt 998 MHz" : "g_volt_e_998400";
        case KipConfigValue_g_volt_e_1036800: return pretty ? "Erista GPU Volt 1036 MHz" : "g_volt_e_1036800";
        case KipConfigValue_g_volt_e_1075200: return pretty ? "Erista GPU Volt 1075 MHz" : "g_volt_e_1075200";
        case KipConfigValue_t6_tRTW_fine_tune: return pretty ? "t6 - tRTW Fine Tune" : "t6_tRTW_fine_fune";
        case KipConfigValue_t7_tWTR_fine_tune: return pretty ? "t7 - tWTR Fine Tune" : "t7_tWTR_fine_tune";
        case KipCrc32:
            return pretty ? "CRC32" : "crc32";
        case HocClkConfigValue_IsFirstLoad:
            return pretty ? "Is First Load" : "is_first_load";
        default:
            return pretty ? "[cfg] no enum format string" : "err_no_format_string";
    }
}

static inline uint64_t hocclkDefaultConfigValue(HocClkConfigValue val)
{
    switch(val)
    {
        case HocClkConfigValue_PollingIntervalMs:
            return 300ULL;
        case HocClkConfigValue_TempLogIntervalMs:
        case HocClkConfigValue_FreqLogIntervalMs:
        case HocClkConfigValue_PowerLogIntervalMs:
        case HocClkConfigValue_CsvWriteIntervalMs:
        case HocClkConfigValue_UncappedClocks:
        case HocClkConfigValue_OverwriteBoostMode:
        case HocClkConfigValue_BatteryChargeCurrent:
        case HocClkConfigValue_OverwriteRefreshRate:
        case HocClkConfigValue_GPUScheduling:
        case HocClkConfigValue_LiveCpuUv:
        case HocClkConfigValue_GPUSchedulingMethod:
        case HocClkConfigValue_MemoryFrequencyMeasurementMode:
            return 0ULL;
        case HocClkConfigValue_RamDisplayUnit:
            return (uint64_t)RamDisplayUnit_MHz;
        case HocClkConfigValue_EristaMaxCpuClock:
            return 1785ULL;

        case HocClkConfigValue_MarikoMaxCpuClock:
            return 1963ULL;

        case HocClkConfigValue_ThermalThrottle:
        case HocClkConfigValue_HandheldTDP:
        case HocClkConfigValue_IsFirstLoad:
        case HocClkConfigValue_DVFSMode:
            return 1ULL;
        case HocClkConfigValue_ThermalThrottleThreshold:
            return 70ULL;
        case HocClkConfigValue_HandheldTDPLimit:
            return 9600ULL; // 8600mW will trigger on erista stock, so raise it a bit
        case HocClkConfigValue_LiteTDPLimit:
            return 6400ULL; // 0.5C
        case HocClkConfigValue_CpuGovernorMinimumFreq:
            return 612000000ULL; // 612MHz
        case HocClkConfigValue_MaxDisplayClockH:
            return 60ULL;
        case HocClkConfigValue_DisplayVoltage:
            return 1200ULL; // Auto
        default:
            return 0ULL;
    }
}

static inline uint64_t hocclkValidConfigValue(HocClkConfigValue val, uint64_t input)
{
    switch(val)
    {
        case HocClkConfigValue_EristaMaxCpuClock:
        case HocClkConfigValue_MarikoMaxCpuClock:
        case HocClkConfigValue_ThermalThrottleThreshold:
        case HocClkConfigValue_HandheldTDPLimit:
        case HocClkConfigValue_LiteTDPLimit:
        case HocClkConfigValue_PollingIntervalMs:
        case HocClkConfigValue_MaxDisplayClockH:
            return input > 0;

        case HocClkConfigValue_TempLogIntervalMs:
        case HocClkConfigValue_FreqLogIntervalMs:
        case HocClkConfigValue_PowerLogIntervalMs:
        case HocClkConfigValue_CsvWriteIntervalMs:
        case HocClkConfigValue_UncappedClocks:
        case HocClkConfigValue_OverwriteBoostMode:
        case HocClkConfigValue_ThermalThrottle:
        case HocClkConfigValue_HandheldTDP:
        case HocClkConfigValue_OverwriteRefreshRate:
        case HocClkConfigValue_IsFirstLoad:
        case HocClkConfigValue_EnableExperimentalSettings:
        case HocClkConfigValue_LiveCpuUv:
        case HocClkConfigValue_GPUSchedulingMethod:
            return (input & 0x1) == input;

        case KipConfigValue_custRev:
        // case KipConfigValue_mtcConf:
        case KipConfigValue_hpMode:
        case KipConfigValue_commonEmcMemVolt:
        case KipConfigValue_eristaEmcMaxClock:
        case KipConfigValue_eristaEmcMaxClock1:
        case KipConfigValue_eristaEmcMaxClock2:
        case KipConfigValue_marikoEmcMaxClock:
        case KipConfigValue_marikoEmcVddqVolt:
        case KipConfigValue_emcDvbShift:
        case KipConfigValue_t1_tRCD:
        case KipConfigValue_t2_tRP:
        case KipConfigValue_t3_tRAS:
        case KipConfigValue_t4_tRRD:
        case KipConfigValue_t5_tRFC:
        case KipConfigValue_t6_tRTW:
        case KipConfigValue_t7_tWTR:
        case KipConfigValue_t8_tREFI:
        case KipConfigValue_mem_burst_read_latency:
        case KipConfigValue_mem_burst_write_latency:
        case KipConfigValue_eristaCpuUV:
        case KipConfigValue_eristaCpuMaxVolt:
        case KipConfigValue_marikoCpuUVLow:
        case KipConfigValue_marikoCpuUVHigh:
        case KipConfigValue_tableConf:
        case KipConfigValue_marikoCpuLowVmin:
        case KipConfigValue_marikoCpuHighVmin:
        case KipConfigValue_marikoCpuMaxVolt:
        case KipConfigValue_eristaCpuBoostClock:
        case KipConfigValue_marikoCpuBoostClock:
        case KipConfigValue_marikoCpuMaxClock:
        case KipConfigValue_eristaGpuUV:
        case KipConfigValue_eristaGpuVmin:
        case KipConfigValue_marikoGpuUV:
        case KipConfigValue_marikoGpuVmin:
        case KipConfigValue_marikoGpuVmax:
        case KipConfigValue_commonGpuVoltOffset:
        case KipConfigValue_gpuSpeedo:
        case KipConfigValue_g_volt_76800:
        case KipConfigValue_g_volt_153600:
        case KipConfigValue_g_volt_230400:
        case KipConfigValue_g_volt_307200:
        case KipConfigValue_g_volt_384000:
        case KipConfigValue_g_volt_460800:
        case KipConfigValue_g_volt_537600:
        case KipConfigValue_g_volt_614400:
        case KipConfigValue_g_volt_691200:
        case KipConfigValue_g_volt_768000:
        case KipConfigValue_g_volt_844800:
        case KipConfigValue_g_volt_921600:
        case KipConfigValue_g_volt_998400:
        case KipConfigValue_g_volt_1075200:
        case KipConfigValue_g_volt_1152000:
        case KipConfigValue_g_volt_1228800:
        case KipConfigValue_g_volt_1267200:
        case KipConfigValue_g_volt_1305600:
        case KipConfigValue_g_volt_1344000:
        case KipConfigValue_g_volt_1382400:
        case KipConfigValue_g_volt_1420800:
        case KipConfigValue_g_volt_1459200:
        case KipConfigValue_g_volt_1497600:
        case KipConfigValue_g_volt_1536000:
        case KipConfigValue_g_volt_e_76800:
        case KipConfigValue_g_volt_e_115200:
        case KipConfigValue_g_volt_e_153600:
        case KipConfigValue_g_volt_e_192000:
        case KipConfigValue_g_volt_e_230400:
        case KipConfigValue_g_volt_e_268800:
        case KipConfigValue_g_volt_e_307200:
        case KipConfigValue_g_volt_e_345600:
        case KipConfigValue_g_volt_e_384000:
        case KipConfigValue_g_volt_e_422400:
        case KipConfigValue_g_volt_e_460800:
        case KipConfigValue_g_volt_e_499200:
        case KipConfigValue_g_volt_e_537600:
        case KipConfigValue_g_volt_e_576000:
        case KipConfigValue_g_volt_e_614400:
        case KipConfigValue_g_volt_e_652800:
        case KipConfigValue_g_volt_e_691200:
        case KipConfigValue_g_volt_e_729600:
        case KipConfigValue_g_volt_e_768000:
        case KipConfigValue_g_volt_e_806400:
        case KipConfigValue_g_volt_e_844800:
        case KipConfigValue_g_volt_e_883200:
        case KipConfigValue_g_volt_e_921600:
        case KipConfigValue_g_volt_e_960000:
        case KipConfigValue_g_volt_e_998400:
        case KipConfigValue_g_volt_e_1036800:
        case KipConfigValue_g_volt_e_1075200:
        case KipConfigValue_eristaCpuVmin:
        case KipConfigValue_eristaCpuUnlock:
        case KipConfigValue_t6_tRTW_fine_tune:
        case KipConfigValue_t7_tWTR_fine_tune:
        case KipCrc32:
        case HocClkConfigValue_DVFSMode:
        case HocClkConfigValue_DVFSOffset:
        case HocClkConfigValue_GPUScheduling:
        case HocClkConfigValue_RAMVoltDisplayMode:
        case HocClkConfigValue_CpuGovernorMinimumFreq:
        case HocClkConfigValue_MemoryFrequencyMeasurementMode:
        case HocClkConfigValue_RamDisplayUnit:
            return true;
        case HocClkConfigValue_BatteryChargeCurrent:
            return ((input >= 1024) && (input <= 3072)) || !input;
        case HocClkConfigValue_DisplayVoltage:
            return ((input >= 800) && (input <= 1325));

        default:
            return false;
    }
}