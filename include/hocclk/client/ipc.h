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

#include "types.h"
#include "../config.h"
#include "../board.h"
#include "../ipc.h"

bool hocclkIpcRunning();
Result hocclkIpcInitialize(void);
void hocclkIpcExit(void);

Result hocclkIpcGetAPIVersion(u32* out_ver);
Result hocclkIpcGetVersionString(char* out, size_t len);
Result hocclkIpcGetCurrentContext(HocClkContext* out_context);
Result hocclkIpcGetProfileCount(u64 tid, u8* out_count);
Result hocclkIpcSetEnabled(bool enabled);
Result hocclkIpcExitCmd();
Result hocclkIpcSetOverride(HocClkModule module, u32 hz);
Result hocclkIpcGetProfiles(u64 tid, HocClkTitleProfileList* out_profiles);
Result hocclkIpcSetProfiles(u64 tid, HocClkTitleProfileList* profiles);
Result hocclkIpcGetConfigValues(HocClkConfigValueList* out_configValues);
Result hocclkIpcSetConfigValues(HocClkConfigValueList* configValues);
Result hocclkIpcGetFreqList(HocClkModule module, u32* list, u32 maxCount, u32* outCount);
Result hocClkIpcSetKipData();
Result hocClkIpcGetKipData();

static inline Result hocclkIpcRemoveOverride(HocClkModule module)
{
    return hocclkIpcSetOverride(module, 0);
}
