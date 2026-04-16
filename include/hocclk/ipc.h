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
#include "clock_manager.h"

#define HOCCLK_IPC_API_VERSION 2
#define HOCCLK_IPC_SERVICE_NAME "hoc:clk"

enum HocClkIpcCmd
{
    HocClkIpcCmd_GetApiVersion = 0,
    HocClkIpcCmd_GetVersionString = 1,
    HocClkIpcCmd_GetCurrentContext = 2,
    HocClkIpcCmd_Exit = 3,
    HocClkIpcCmd_GetProfileCount = 4,
    HocClkIpcCmd_GetProfiles = 5,
    HocClkIpcCmd_SetProfiles = 6,
    HocClkIpcCmd_SetEnabled = 7,
    HocClkIpcCmd_SetOverride = 8,
    HocClkIpcCmd_GetConfigValues = 9,
    HocClkIpcCmd_SetConfigValues = 10,
    HocClkIpcCmd_GetFreqList = 11,
    HocClkIpcCmd_SetKipData = 12,
    HocClkIpcCmd_GetKipData = 13,
};


typedef struct
{
    uint64_t tid;
    HocClkTitleProfileList profiles;
} HocClkIpc_SetProfiles_Args;

typedef struct
{
    HocClkModule module;
    uint32_t hz;
} HocClkIpc_SetOverride_Args;

typedef struct
{
    HocClkModule module;
    uint32_t maxCount;
} HocClkIpc_GetFreqList_Args;