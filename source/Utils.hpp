#pragma once
#include "SaltyNX.h"

#include "Battery.hpp"
#include "audsnoop.h"
#include "Misc.hpp"
#include "i2c.h"
#include "max17050.h"
#include <numeric>
#include <tesla.hpp>
#include <sys/stat.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#include <sysclk/client/ipc.h>

#if defined(__cplusplus)
}
#endif

#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715
#define FieldDescriptor uint32_t
#define BASE_SNS_UOHM 5000

//Common
Thread t0;
Thread t1;
Thread t2;
Thread t3;
Thread t4;
Thread t6;
Thread t7;
const uint64_t systemtickfrequency = 19200000;
bool threadexit = false;
bool threadexit2 = false;
FanController g_ICon;
std::string folderpath = "sdmc:/switch/.overlays/";
std::string filename = "";
std::string filepath = "";
std::string keyCombo = "L+DDOWN+RSTICK"; // default Tesla Menu combo

//Misc2
MmuRequest nvdecRequest;
MmuRequest nvencRequest;
MmuRequest nvjpgRequest;

//Checks
Result clkrstCheck = 1;
Result nvCheck = 1;
Result pcvCheck = 1;
Result tsCheck = 1;
Result fanCheck = 1;
Result tcCheck = 1;
Result Hinted = 1;
Result pmdmntCheck = 1;
Result psmCheck = 1;
Result audsnoopCheck = 1;
Result nvdecCheck = 1;
Result nvencCheck = 1;
Result nvjpgCheck = 1;
Result nifmCheck = 1;
Result sysclkCheck = 1;

//Wi-Fi
NifmInternetConnectionType NifmConnectionType = (NifmInternetConnectionType)-1;
NifmInternetConnectionStatus NifmConnectionStatus = (NifmInternetConnectionStatus)-1;
bool Nifm_showpass = false;
Result Nifm_internet_rc = -1;
Result Nifm_profile_rc = -1;
NifmNetworkProfileData_new Nifm_profile = {0};

//Multimedia engines
uint32_t NVDEC_Hz = 0;
uint32_t NVENC_Hz = 0;
uint32_t NVJPG_Hz = 0;

//DSP
uint32_t DSP_Load_u = -1;

//Battery
Service* psmService = 0;
BatteryChargeInfoFields _batteryChargeInfoFields = {0};
float batCurrentAvg = 0;
float batVoltageAvg = 0;
float PowerConsumption = 0;
int16_t batTimeEstimate = -1;
float actualFullBatCapacity = 0;
float designedFullBatCapacity = 0;
bool batteryFiltered = false;
uint8_t batteryTimeLeftRefreshRate = 60;

//Temperatures
float SOC_temperatureF = 0;
float PCB_temperatureF = 0;
int32_t SOC_temperatureC = 0;
int32_t PCB_temperatureC = 0;
int32_t skin_temperaturemiliC = 0;

//CPU Usage
uint64_t idletick0 = systemtickfrequency;
uint64_t idletick1 = systemtickfrequency;
uint64_t idletick2 = systemtickfrequency;
uint64_t idletick3 = systemtickfrequency;

//Frequency
uint32_t CPU_Hz = 0;
uint32_t GPU_Hz = 0;
uint32_t RAM_Hz = 0;

//RAM Size
uint64_t RAM_Total_all_u = 0;
uint64_t RAM_Total_application_u = 0;
uint64_t RAM_Total_applet_u = 0;
uint64_t RAM_Total_system_u = 0;
uint64_t RAM_Total_systemunsafe_u = 0;
uint64_t RAM_Used_all_u = 0;
uint64_t RAM_Used_application_u = 0;
uint64_t RAM_Used_applet_u = 0;
uint64_t RAM_Used_system_u = 0;
uint64_t RAM_Used_systemunsafe_u = 0;

//Fan
float Rotation_SpeedLevel_f = 0;

//GPU Usage
FieldDescriptor fd = 0;
uint32_t GPU_Load_u = 0;

//NX-FPS
bool GameRunning = false;
bool check = true;
bool SaltySD = false;
uintptr_t FPSaddress = 0;
uintptr_t FPSavgaddress = 0;
uint64_t PID = 0;
uint32_t FPS = 0xFE;
float FPSavg = 254;
SharedMemory _sharedmemory = {};
bool SharedMemoryUsed = false;
uint32_t* MAGIC_shared = 0;
uint8_t* FPS_shared = 0;
float* FPSavg_shared = 0;
bool* pluginActive = 0;
uint32_t* FPSticks_shared = 0;
Handle remoteSharedMemory = 1;

//Read real freqs from sys-clk sysmodule
uint32_t realCPU_Hz = 0;
uint32_t realGPU_Hz = 0;
uint32_t realRAM_Hz = 0;
uint32_t ramLoad[SysClkRamLoad_EnumMax];

void LoadSharedMemory() {
	if (SaltySD_Connect())
		return;

	SaltySD_GetSharedMemoryHandle(&remoteSharedMemory);
	SaltySD_Term();

	shmemLoadRemote(&_sharedmemory, remoteSharedMemory, 0x1000, Perm_Rw);
	if (!shmemMap(&_sharedmemory))
		SharedMemoryUsed = true;
	else FPS = 1234;
}

ptrdiff_t searchSharedMemoryBlock(uintptr_t base) {
	ptrdiff_t search_offset = 0;
	while(search_offset < 0x1000) {
		MAGIC_shared = (uint32_t*)(base + search_offset);
		if (*MAGIC_shared == 0x465053) {
			return search_offset;
		}
		else search_offset += 4;
	}
	return -1;
}

//Check if SaltyNX is working
bool CheckPort () {
	Handle saltysd;
	for (int i = 0; i < 67; i++) {
		if (R_SUCCEEDED(svcConnectToNamedPort(&saltysd, "InjectServ"))) {
			svcCloseHandle(saltysd);
			break;
		}
		else {
			if (i == 66) return false;
			svcSleepThread(1'000'000);
		}
	}
	for (int i = 0; i < 67; i++) {
		if (R_SUCCEEDED(svcConnectToNamedPort(&saltysd, "InjectServ"))) {
			svcCloseHandle(saltysd);
			return true;
		}
		else svcSleepThread(1'000'000);
	}
	return false;
}

void CheckIfGameRunning(void*) {
	while (!threadexit2) {
		if (!check && R_FAILED(pmdmntGetApplicationProcessId(&PID))) {
			GameRunning = false;
			if (SharedMemoryUsed) {
				*MAGIC_shared = 0;
				*pluginActive = false;
				*FPS_shared = 0;
				*FPSavg_shared = 0.0;
				FPS = 254;
				FPSavg = 254.0;
			}
			check = true;
		}
		else if (!GameRunning && SharedMemoryUsed) {
				uintptr_t base = (uintptr_t)shmemGetAddr(&_sharedmemory);
				ptrdiff_t rel_offset = searchSharedMemoryBlock(base);
				if (rel_offset > -1) {
					FPS_shared = (uint8_t*)(base + rel_offset + 4);
					FPSavg_shared = (float*)(base + rel_offset + 5);
					pluginActive = (bool*)(base + rel_offset + 9);
					FPSticks_shared = (uint32_t*)(base + rel_offset + 15);
					*pluginActive = false;
					svcSleepThread(100'000'000);
					if (*pluginActive) {
						GameRunning = true;
						check = false;
					}
				}
		}
		svcSleepThread(1'000'000'000);
	}
}

Mutex mutex_BatteryChecker = {0};
void BatteryChecker(void*) {
	if (R_FAILED(psmCheck)){
		return;
	}
	uint16_t data = 0;
	float tempV = 0.0;
	float tempA = 0.0;
	size_t ArraySize = 10;
	if (batteryFiltered) {
		ArraySize = 1;
	}
	float* readingsAmp = new float[ArraySize];
	float* readingsVolt = new float[ArraySize];

	Max17050ReadReg(MAX17050_AvgCurrent, &data);
	tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
	for (size_t i = 0; i < ArraySize; i++) {
		readingsAmp[i] = tempA;
	}
	Max17050ReadReg(MAX17050_AvgVCELL, &data);
	tempV = 0.625 * (data >> 3);
	for (size_t i = 0; i < ArraySize; i++) {
		readingsVolt[i] = tempV;
	}
	if (!actualFullBatCapacity) {
		Max17050ReadReg(MAX17050_FullCAP, &data);
		actualFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
	}
	if (!designedFullBatCapacity) {
		Max17050ReadReg(MAX17050_DesignCap, &data);
		designedFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
	}
	if (readingsAmp[0] >= 0) {
		batTimeEstimate = -1;
	}
	else {
		Max17050ReadReg(MAX17050_TTE, &data);
		float batteryTimeEstimateInMinutes = (5.625 * data) / 60;
		if (batteryTimeEstimateInMinutes > (99.0*60.0)+59.0) {
			batTimeEstimate = (99*60)+59;
		}
		else batTimeEstimate = (int16_t)batteryTimeEstimateInMinutes;
	}

	size_t counter = 0;
	uint64_t tick_TTE = svcGetSystemTick();
	while (!threadexit) {
		mutexLock(&mutex_BatteryChecker);
		uint64_t startTick = svcGetSystemTick();

		psmGetBatteryChargeInfoFields(psmService, &_batteryChargeInfoFields);

		// Calculation is based on Hekate's max17050.c
		// Source: https://github.com/CTCaer/hekate/blob/master/bdk/power/max17050.c

		if (!batteryFiltered) {
			Max17050ReadReg(MAX17050_Current, &data);
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			Max17050ReadReg(MAX17050_VCELL, &data);
			tempV = 0.625 * (data >> 3);
		} else {
			Max17050ReadReg(MAX17050_AvgCurrent, &data);
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			Max17050ReadReg(MAX17050_AvgVCELL, &data);
			tempV = 0.625 * (data >> 3);
		}

		if (tempA && tempV) {
			readingsAmp[counter % ArraySize] = tempA;
			readingsVolt[counter % ArraySize] = tempV;
			counter++;
		}

		float batCurrent = 0.0;
		float batVoltage = 0.0;
		float batPowerAvg = 0.0;
		for (size_t x = 0; x < ArraySize; x++) {
			batCurrent += readingsAmp[x];
			batVoltage += readingsVolt[x];
			batPowerAvg += (readingsAmp[x] * readingsVolt[x]) / 1'000;
		}
		batCurrent /= ArraySize;
		batVoltage /= ArraySize;
		batCurrentAvg = batCurrent;
		batVoltageAvg = batVoltage;
		batPowerAvg /= ArraySize * 1000;
		PowerConsumption = batPowerAvg;

		if (batCurrentAvg >= 0) {
			batTimeEstimate = -1;
		} 
		else {
			static float batteryTimeEstimateInMinutes = 0;
			Max17050ReadReg(MAX17050_TTE, &data);
			batteryTimeEstimateInMinutes = (5.625 * data) / 60;
			if (batteryTimeEstimateInMinutes > (99.0*60.0)+59.0) {
				batteryTimeEstimateInMinutes = (99.0*60.0)+59.0;
			}
			uint64_t new_tick_TTE = svcGetSystemTick();
			if (armTicksToNs(new_tick_TTE - tick_TTE) / 1'000'000'000 >= batteryTimeLeftRefreshRate) {
				batTimeEstimate = (int16_t)batteryTimeEstimateInMinutes;
				tick_TTE = new_tick_TTE;
			}
		}

		mutexUnlock(&mutex_BatteryChecker);
		uint64_t nanosecondsPassed = armTicksToNs(svcGetSystemTick() - startTick);
		if (nanosecondsPassed < 1'000'000'000 / 2) {
			svcSleepThread((1'000'000'000 / 2) - nanosecondsPassed);
		} else {
			svcSleepThread(1'000);
		}
	}
	batTimeEstimate = -1;
	_batteryChargeInfoFields = {0};
	delete[] readingsAmp;
	delete[] readingsVolt;
}

void StartBatteryThread() {
	threadCreate(&t7, BatteryChecker, NULL, NULL, 0x4000, 0x3F, 3);
	threadStart(&t7);
}

Mutex mutex_Misc = {0};
//Stuff that doesn't need multithreading
void Misc(void*) {
	while (!threadexit) {
		mutexLock(&mutex_Misc);
		// CPU, GPU and RAM Frequency
		if (R_SUCCEEDED(clkrstCheck)) {
			ClkrstSession clkSession;
			if (R_SUCCEEDED(clkrstOpenSession(&clkSession, PcvModuleId_CpuBus, 3))) {
				clkrstGetClockRate(&clkSession, &CPU_Hz);
				clkrstCloseSession(&clkSession);
			}
			if (R_SUCCEEDED(clkrstOpenSession(&clkSession, PcvModuleId_GPU, 3))) {
				clkrstGetClockRate(&clkSession, &GPU_Hz);
				clkrstCloseSession(&clkSession);
			}
			if (R_SUCCEEDED(clkrstOpenSession(&clkSession, PcvModuleId_EMC, 3))) {
				clkrstGetClockRate(&clkSession, &RAM_Hz);
				clkrstCloseSession(&clkSession);
			}
		}
		else if (R_SUCCEEDED(pcvCheck)) {
			pcvGetClockRate(PcvModule_CpuBus, &CPU_Hz);
			pcvGetClockRate(PcvModule_GPU, &GPU_Hz);
			pcvGetClockRate(PcvModule_EMC, &RAM_Hz);
		}
		if (R_SUCCEEDED(sysclkCheck)) {
			SysClkContext sysclkCTX;
			if (R_SUCCEEDED(sysclkIpcGetCurrentContext(&sysclkCTX))) {
				realCPU_Hz = sysclkCTX.realFreqs[SysClkModule_CPU];
				realGPU_Hz = sysclkCTX.realFreqs[SysClkModule_GPU];
				realRAM_Hz = sysclkCTX.realFreqs[SysClkModule_MEM];
				ramLoad[SysClkRamLoad_All] = sysclkCTX.ramLoad[SysClkRamLoad_All];
				ramLoad[SysClkRamLoad_Cpu] = sysclkCTX.ramLoad[SysClkRamLoad_Cpu];
			}
		}
		
		//Temperatures
		if (R_SUCCEEDED(tsCheck)) {
			if (hosversionAtLeast(10,0,0)) {
				TsSession ts_session;
				Result rc = tsOpenSession(&ts_session, TsDeviceCode_LocationExternal);
				if (R_SUCCEEDED(rc)) {
					tsSessionGetTemperature(&ts_session, &SOC_temperatureF);
					tsSessionClose(&ts_session);
				}
				rc = tsOpenSession(&ts_session, TsDeviceCode_LocationInternal);
				if (R_SUCCEEDED(rc)) {
					tsSessionGetTemperature(&ts_session, &PCB_temperatureF);
					tsSessionClose(&ts_session);
				}
			}
			else {
				tsGetTemperatureMilliC(TsLocation_External, &SOC_temperatureC);
				tsGetTemperatureMilliC(TsLocation_Internal, &PCB_temperatureC);
			}
		}
		if (R_SUCCEEDED(tcCheck)) tcGetSkinTemperatureMilliC(&skin_temperaturemiliC);
		
		//RAM Memory Used
		if (R_SUCCEEDED(Hinted)) {
			svcGetSystemInfo(&RAM_Total_application_u, 0, INVALID_HANDLE, 0);
			svcGetSystemInfo(&RAM_Total_applet_u, 0, INVALID_HANDLE, 1);
			svcGetSystemInfo(&RAM_Total_system_u, 0, INVALID_HANDLE, 2);
			svcGetSystemInfo(&RAM_Total_systemunsafe_u, 0, INVALID_HANDLE, 3);
			svcGetSystemInfo(&RAM_Used_application_u, 1, INVALID_HANDLE, 0);
			svcGetSystemInfo(&RAM_Used_applet_u, 1, INVALID_HANDLE, 1);
			svcGetSystemInfo(&RAM_Used_system_u, 1, INVALID_HANDLE, 2);
			svcGetSystemInfo(&RAM_Used_systemunsafe_u, 1, INVALID_HANDLE, 3);
		}
		
		//Fan
		if (R_SUCCEEDED(fanCheck)) fanControllerGetRotationSpeedLevel(&g_ICon, &Rotation_SpeedLevel_f);
		
		//GPU Load
		if (R_SUCCEEDED(nvCheck)) nvIoctl(fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &GPU_Load_u);
		
		//FPS
		if (GameRunning) {
			if (SharedMemoryUsed) {
				FPS = *FPS_shared;
				FPSavg = 19'200'000.f / (std::accumulate<uint32_t*, float>(FPSticks_shared, FPSticks_shared+10, 0) / 10);
			}
		}
		else FPSavg = 254;
		
		// Interval
		mutexUnlock(&mutex_Misc);
		svcSleepThread(TeslaFPS < 10 ? (1'000'000'000 / TeslaFPS) : 100'000'000);
	}
}

void Misc2(void*) {
	while (!threadexit) {
		//DSP
		if (R_SUCCEEDED(audsnoopCheck)) audsnoopGetDspUsage(&DSP_Load_u);

		//Multimedia clock rates
		if (R_SUCCEEDED(nvdecCheck)) mmuRequestGet(&nvdecRequest, &NVDEC_Hz);
		if (R_SUCCEEDED(nvencCheck)) mmuRequestGet(&nvencRequest, &NVENC_Hz);
		if (R_SUCCEEDED(nvjpgCheck)) mmuRequestGet(&nvjpgRequest, &NVJPG_Hz);

		if (R_SUCCEEDED(nifmCheck)) {
			u32 dummy = 0;
			Nifm_internet_rc = nifmGetInternetConnectionStatus(&NifmConnectionType, &dummy, &NifmConnectionStatus);
			if (!Nifm_internet_rc && (NifmConnectionType == NifmInternetConnectionType_WiFi))
				Nifm_profile_rc = nifmGetCurrentNetworkProfile((NifmNetworkProfileData*)&Nifm_profile);
		}
		// Interval
		svcSleepThread(100'000'000);
	}
}

//Check each core for idled ticks in intervals, they cannot read info about other core than they are assigned
//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
//This is because making each loop also takes time, which is not considered because this will take also additional time
void CheckCore0(void*) {
	while (!threadexit) {
		uint64_t idletick_a0 = 0;
		uint64_t idletick_b0 = 0;
		svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		svcSleepThread(1'000'000'000 / TeslaFPS);
		svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		idletick0 = idletick_a0 - idletick_b0;
	}
}

void CheckCore1(void*) {
	while (!threadexit) {
		uint64_t idletick_a1 = 0;
		uint64_t idletick_b1 = 0;
		svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		svcSleepThread(1'000'000'000 / TeslaFPS);
		svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		idletick1 = idletick_a1 - idletick_b1;
	}
}

void CheckCore2(void*) {
	while (!threadexit) {
		uint64_t idletick_a2 = 0;
		uint64_t idletick_b2 = 0;
		svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		svcSleepThread(1'000'000'000 / TeslaFPS);
		svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		idletick2 = idletick_a2 - idletick_b2;
	}
}

void CheckCore3(void*) {
	while (!threadexit) {
		uint64_t idletick_a3 = 0;
		uint64_t idletick_b3 = 0;
		svcGetInfo(&idletick_b3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
		svcSleepThread(1'000'000'000 / TeslaFPS);
		svcGetInfo(&idletick_a3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
		idletick3 = idletick_a3 - idletick_b3;
	}
}

//Start reading all stats
void StartThreads() {
	threadCreate(&t0, CheckCore0, NULL, NULL, 0x1000, 0x10, 0);
	threadCreate(&t1, CheckCore1, NULL, NULL, 0x1000, 0x10, 1);
	threadCreate(&t2, CheckCore2, NULL, NULL, 0x1000, 0x10, 2);
	threadCreate(&t3, CheckCore3, NULL, NULL, 0x1000, 0x10, 3);
	threadCreate(&t4, Misc, NULL, NULL, 0x1000, 0x3F, -2);
	if (SaltySD) {
		//Assign NX-FPS to default core
		threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
	}
				
	threadStart(&t0);
	threadStart(&t1);
	threadStart(&t2);
	threadStart(&t3);
	threadStart(&t4);
	if (SaltySD) {
		//Start NX-FPS detection
		threadStart(&t6);
	}
	StartBatteryThread();
}

//End reading all stats
void CloseThreads() {
	threadexit = true;
	threadexit2 = true;
	threadWaitForExit(&t0);
	threadWaitForExit(&t1);
	threadWaitForExit(&t2);
	threadWaitForExit(&t3);
	threadWaitForExit(&t4);
	threadWaitForExit(&t6);
	threadWaitForExit(&t7);
	threadClose(&t0);
	threadClose(&t1);
	threadClose(&t2);
	threadClose(&t3);
	threadClose(&t4);
	threadClose(&t6);
	threadClose(&t7);
	threadexit = false;
	threadexit2 = false;
}

//Separate functions dedicated to "FPS Counter" mode
void FPSCounter(void*) {
	while (!threadexit) {
		if (GameRunning) {
			if (SharedMemoryUsed) {
				FPS = *FPS_shared;
				FPSavg = 19'200'000.f / (std::accumulate<uint32_t*, float>(FPSticks_shared, FPSticks_shared+10, 0) / 10);
			}
		}
		else FPSavg = 254;
		svcSleepThread(1'000'000'000 / TeslaFPS);
	}
}

void StartFPSCounterThread() {
	//Assign NX-FPS to default core
	threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
	threadCreate(&t0, FPSCounter, NULL, NULL, 0x1000, 0x3F, 3);
	threadStart(&t0);
	threadStart(&t6);
}

void EndFPSCounterThread() {
	threadexit = true;
	threadexit2 = true;
	threadWaitForExit(&t0);
	threadClose(&t0);
	threadWaitForExit(&t6);
	threadClose(&t6);
	threadexit = false;
	threadexit2 = false;
}


// String formatting functions
void removeSpaces(std::string& str) {
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

void convertToUpper(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void convertToLower(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void formatButtonCombination(std::string& line) {
	std::map<std::string, std::string> replaces{
		{"A", "\uE0E0"},
		{"B", "\uE0E1"},
		{"X", "\uE0E2"},
		{"Y", "\uE0E3"},
		{"L", "\uE0E4"},
		{"R", "\uE0E5"},
		{"ZL", "\uE0E6"},
		{"ZR", "\uE0E7"},
		{"SL", "\uE0E8"},
		{"SR", "\uE0E9"},
		{"DUP", "\uE0EB"},
		{"DDOWN", "\uE0EC"},
		{"DLEFT", "\uE0ED"},
		{"DRIGHT", "\uE0EE"},
		{"PLUS", "\uE0EF"},
		{"MINUS", "\uE0F0"},
		{"LSTICK", "\uE104"},
		{"RSTICK", "\uE105"},
		{"RS", "\uE105"},
		{"LS", "\uE104"}
	};
	// Remove all spaces from the line
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

	// Replace '+' with ' + '
	size_t pos = 0;
	size_t max_pluses = 3;
	while ((pos = line.find('+', pos)) != std::string::npos) {
		if (!max_pluses) {
			line = line.substr(0, pos);
			return;
		}
		if (pos > 0 && pos < line.size() - 1) {
			if (std::isalnum(line[pos - 1]) && std::isalnum(line[pos + 1])) {
				line.replace(pos, 1, " + ");
				pos += 3;
			}
		}
		++pos;
		max_pluses--;
	}
	pos = 0;
	size_t old_pos = 0;
	while ((pos = line.find(" + ", pos)) != std::string::npos) {

		std::string button = line.substr(old_pos, pos - old_pos);
		if (replaces.find(button) != replaces.end()) {
			line.replace(old_pos, button.length(), replaces[button]);
			pos = 0;
			old_pos = 0;
		}
		else pos += 3;
		old_pos = pos;
	}
	std::string button = line.substr(old_pos);
	if (replaces.find(button) != replaces.end()) {
		line.replace(old_pos, button.length(), replaces[button]);
	}	
}

uint64_t MapButtons(const std::string& buttonCombo) {
	std::map<std::string, uint64_t> buttonMap = {
		{"A", HidNpadButton_A},
		{"B", HidNpadButton_B},
		{"X", HidNpadButton_X},
		{"Y", HidNpadButton_Y},
		{"L", HidNpadButton_L},
		{"R", HidNpadButton_R},
		{"ZL", HidNpadButton_ZL},
		{"ZR", HidNpadButton_ZR},
		{"PLUS", HidNpadButton_Plus},
		{"MINUS", HidNpadButton_Minus},
		{"DUP", HidNpadButton_Up},
		{"DDOWN", HidNpadButton_Down},
		{"DLEFT", HidNpadButton_Left},
		{"DRIGHT", HidNpadButton_Right},
		{"SL", HidNpadButton_AnySL},
		{"SR", HidNpadButton_AnySR},
		{"LSTICK", HidNpadButton_StickL},
		{"RSTICK", HidNpadButton_StickR},
		{"LS", HidNpadButton_StickL},
		{"RS", HidNpadButton_StickR},
		{"UP", HidNpadButton_AnyUp},
		{"DOWN", HidNpadButton_AnyDown},
		{"LEFT", HidNpadButton_AnyLeft},
		{"RIGHT", HidNpadButton_AnyRight}
	};

	uint64_t comboBitmask = 0;
	std::string comboCopy = buttonCombo;  // Make a copy of buttonCombo

	std::string delimiter = "+";
	size_t pos = 0;
	std::string button;
	size_t max_delimiters = 4;
	while ((pos = comboCopy.find(delimiter)) != std::string::npos) {
		button = comboCopy.substr(0, pos);
		if (buttonMap.find(button) != buttonMap.end()) {
			comboBitmask |= buttonMap[button];
		}
		comboCopy.erase(0, pos + delimiter.length());
		if (!--max_delimiters) {
			return comboBitmask;
		}
	}
	if (buttonMap.find(comboCopy) != buttonMap.end()) {
		comboBitmask |= buttonMap[comboCopy];
	}
	return comboBitmask;
}

static inline bool isKeyComboPressed(uint64_t keysHeld, uint64_t keysDown, uint64_t comboBitmask) {
	return (keysDown == comboBitmask) || (keysHeld == comboBitmask);
}

// Custom utility function for parsing an ini file
void ParseIniFile() {
	std::string overlayName;
	std::string directoryPath = "sdmc:/config/status-monitor/";
	std::string ultrahandDirectoryPath = "sdmc:/config/ultrahand/";
	std::string teslaDirectoryPath = "sdmc:/config/tesla/";
	std::string configIniPath = directoryPath + "config.ini";
	std::string ultrahandConfigIniPath = ultrahandDirectoryPath + "config.ini";
	std::string teslaConfigIniPath = teslaDirectoryPath + "config.ini";
	tsl::hlp::ini::IniData parsedData;
	
	struct stat st;
	if (stat(directoryPath.c_str(), &st) != 0) {
		mkdir(directoryPath.c_str(), 0777);
	}

	
	bool readExternalCombo = false;
	// Open the INI file
	FILE* configFileIn = fopen(configIniPath.c_str(), "r");
	if (configFileIn) {
		// Determine the size of the INI file
		fseek(configFileIn, 0, SEEK_END);
		long fileSize = ftell(configFileIn);
		rewind(configFileIn);
			
		// Parse the INI data
		std::string fileDataString(fileSize, '\0');
		fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
		fclose(configFileIn);

		parsedData = tsl::hlp::ini::parseIni(fileDataString);
		
		// Access and use the parsed data as needed
		// For example, print the value of a specific section and key
		if (parsedData.find("status-monitor") != parsedData.end()) {
			if (parsedData["status-monitor"].find("key_combo") != parsedData["status-monitor"].end()) {
				keyCombo = parsedData["status-monitor"]["key_combo"]; // load keyCombo variable
				removeSpaces(keyCombo); // format combo
				convertToUpper(keyCombo);
			} 
			else {
				readExternalCombo = true;
			}
			if (parsedData["status-monitor"].find("battery_avg_iir_filter") != parsedData["status-monitor"].end()) {
				auto key = parsedData["status-monitor"]["battery_avg_iir_filter"];
				convertToUpper(key);
				batteryFiltered = !key.compare("TRUE");
			}
			if (parsedData["status-monitor"].find("battery_time_left_refreshrate") != parsedData["status-monitor"].end()) {
				auto key = parsedData["status-monitor"]["battery_time_left_refreshrate"];
				long maxSeconds = 60;
				long minSeconds = 1;
		
				long rate = atol(key.c_str());

				if (rate > maxSeconds) {
					rate = maxSeconds;
				}
				else if (rate < minSeconds) {
					rate = minSeconds;
				}
				batteryTimeLeftRefreshRate = rate;
			}
		}
		
	} else {
		readExternalCombo = true;
	}

	if (readExternalCombo) {
		FILE* ultrahandConfigFileIn = fopen(ultrahandConfigIniPath.c_str(), "r");
		FILE* teslaConfigFileIn = fopen(teslaConfigIniPath.c_str(), "r");
		if (ultrahandConfigFileIn) {
			if (teslaConfigFileIn)
				fclose(teslaConfigFileIn);
			
			// load keyCombo from teslaConfig
			std::string ultrahandFileData;
			char buffer[256];
			while (fgets(buffer, sizeof(buffer), ultrahandConfigFileIn) != NULL) {
				ultrahandFileData += buffer;
			}
			fclose(ultrahandConfigFileIn);
			
			parsedData = tsl::hlp::ini::parseIni(ultrahandFileData);
			if (parsedData.find("ultrahand") != parsedData.end() &&
				parsedData["ultrahand"].find("key_combo") != parsedData["ultrahand"].end()) {
				keyCombo = parsedData["ultrahand"]["key_combo"];
				removeSpaces(keyCombo); // format combo
				convertToUpper(keyCombo);
			}
			
		} else if (teslaConfigFileIn) {
			// load keyCombo from teslaConfig
			std::string teslaFileData;
			char buffer[256];
			while (fgets(buffer, sizeof(buffer), teslaConfigFileIn) != NULL) {
				teslaFileData += buffer;
			}
			fclose(teslaConfigFileIn);
			
			parsedData = tsl::hlp::ini::parseIni(teslaFileData);
			if (parsedData.find("tesla") != parsedData.end() &&
				parsedData["tesla"].find("key_combo") != parsedData["tesla"].end()) {
				keyCombo = parsedData["tesla"]["key_combo"];
				removeSpaces(keyCombo); // format combo
				convertToUpper(keyCombo);
			}
		}
	}
}


bool isValidRGBA4Color(const std::string& hexColor) {
    for (char c : hexColor) {
        if (!isxdigit(c)) {
            return false; // Must contain only hexadecimal digits (0-9, A-F, a-f)
        }
    }
    
    return true;
}

bool convertStrToRGBA4444(std::string hexColor, uint16_t* returnValue) {
	// Check if # is present
	if (hexColor.size() != 5 || hexColor[0] != '#')
		return false;
	
	hexColor = hexColor.substr(1);

	if (isValidRGBA4Color(hexColor)) {
		*returnValue = std::stoi(std::string(hexColor.rbegin(), hexColor.rend()), nullptr, 16);
		return true;
	}
	return false;
}

struct FullSettings {
	uint8_t refreshRate;
	bool setPosRight;
	bool showRealFreqs;
	bool showDeltas;
	bool showTargetFreqs;
};

struct MiniSettings {
	uint8_t refreshRate;
	bool realFrequencies;
	size_t handheldFontSize;
	size_t dockedFontSize;
	uint16_t backgroundColor;
	uint16_t catColor;
	uint16_t textColor;
	std::string show;
	bool showRAMLoad;
	int setPos;
};

struct MicroSettings {
	uint8_t refreshRate;
	bool realFrequencies;
	size_t handheldFontSize;
	size_t dockedFontSize;
	uint8_t alignTo;
	uint16_t backgroundColor;
	uint16_t catColor;
	uint16_t textColor;
	std::string show;
	bool showRAMLoad;
	bool setPosBottom;
};

struct FpsCounterSettings {
	uint8_t refreshRate;
	size_t handheldFontSize;
	size_t dockedFontSize;
	uint16_t backgroundColor;
	uint16_t textColor;
	int setPos;
};

struct FpsGraphSettings {
	uint8_t refreshRate;
	uint16_t backgroundColor;
	uint16_t fpsColor;
	uint16_t mainLineColor;
	uint16_t roundedLineColor;
	uint16_t perfectLineColor;
	uint16_t dashedLineColor;
	uint16_t borderColor;
	uint16_t maxFPSTextColor;
	uint16_t minFPSTextColor;
	int setPos;
};

void GetConfigSettings(MiniSettings* settings) {
	settings -> realFrequencies = false;
	settings -> handheldFontSize = 15;
	settings -> dockedFontSize = 15;
	convertStrToRGBA4444("#1117", &(settings -> backgroundColor));
	convertStrToRGBA4444("#FFFF", &(settings -> catColor));
	convertStrToRGBA4444("#FFFF", &(settings -> textColor));
	settings -> show = "CPU+GPU+RAM+TEMP+DRAW+FAN+FPS";
	settings -> showRAMLoad = true;
	settings -> refreshRate = 1;

	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (parsedData.find("mini") == parsedData.end())
		return;
	if (parsedData["mini"].find("refresh_rate") != parsedData["mini"].end()) {
		long maxFPS = 60;
		long minFPS = 1;

		key = parsedData["mini"]["refresh_rate"];
		long rate = atol(key.c_str());
		if (rate < minFPS) {
			settings -> refreshRate = minFPS;
		}
		else if (rate > maxFPS)
			settings -> refreshRate = maxFPS;
		else settings -> refreshRate = rate;	
	}
	if (parsedData["mini"].find("real_freqs") != parsedData["mini"].end()) {
		key = parsedData["mini"]["real_freqs"];
		convertToUpper(key);
		settings -> realFrequencies = !(key.compare("TRUE"));
	}

	long maxFontSize = 22;
	long minFontSize = 8;
	if (parsedData["mini"].find("handheld_font_size") != parsedData["mini"].end()) {
		key = parsedData["mini"]["handheld_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> handheldFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> handheldFontSize = maxFontSize;
		else settings -> handheldFontSize = fontsize;	
	}
	if (parsedData["mini"].find("docked_font_size") != parsedData["mini"].end()) {
		key = parsedData["mini"]["docked_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> dockedFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> dockedFontSize = maxFontSize;
		else settings -> dockedFontSize = fontsize;	
	}
	if (parsedData["mini"].find("background_color") != parsedData["mini"].end()) {
		key = parsedData["mini"]["background_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> backgroundColor = temp;
	}
	if (parsedData["mini"].find("cat_color") != parsedData["mini"].end()) {
		key = parsedData["mini"]["cat_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> catColor = temp;
	}
	if (parsedData["mini"].find("text_color") != parsedData["mini"].end()) {
		key = parsedData["mini"]["text_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> textColor = temp;
	}
	if (parsedData["mini"].find("show") != parsedData["micro"].end()) {
		key = parsedData["mini"]["show"];
		convertToUpper(key);
		settings -> show = key;
	}
	if (parsedData["mini"].find("replace_MB_with_RAM_load") != parsedData["mini"].end()) {
		key = parsedData["mini"]["replace_MB_with_RAM_load"];
		convertToUpper(key);
		settings -> showRAMLoad = key.compare("FALSE");
	}
	if (parsedData["mini"].find("layer_width_align") != parsedData["mini"].end()) {
		key = parsedData["mini"]["layer_width_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos = 1;
		}
		if (!key.compare("RIGHT")) {
			settings -> setPos = 2;
		}
	}
	if (parsedData["mini"].find("layer_height_align") != parsedData["mini"].end()) {
		key = parsedData["mini"]["layer_height_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos += 3;
		}
		if (!key.compare("BOTTOM")) {
			settings -> setPos += 6;
		}
	}
}

void GetConfigSettings(MicroSettings* settings) {
	settings -> realFrequencies = false;
	settings -> handheldFontSize = 18;
	settings -> dockedFontSize = 18;
	settings -> alignTo = 1;
	convertStrToRGBA4444("#1117", &(settings -> backgroundColor));
	convertStrToRGBA4444("#FCCF", &(settings -> catColor));
	convertStrToRGBA4444("#FFFF", &(settings -> textColor));
	settings -> show = "CPU+GPU+RAM+BRD+FAN+FPS";
	settings -> showRAMLoad = true;
	settings -> setPosBottom = false;
	settings -> refreshRate = 1;

	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (parsedData.find("micro") == parsedData.end())
		return;
	if (parsedData["micro"].find("refresh_rate") != parsedData["micro"].end()) {
		long maxFPS = 60;
		long minFPS = 1;
		
		key = parsedData["micro"]["refresh_rate"];
		long rate = atol(key.c_str());
		if (rate < minFPS) {
			settings -> refreshRate = minFPS;
		}
		else if (rate > maxFPS)
			settings -> refreshRate = maxFPS;
		else settings -> refreshRate = rate;	
	}
	if (parsedData["micro"].find("real_freqs") != parsedData["micro"].end()) {
		key = parsedData["micro"]["real_freqs"];
		convertToUpper(key);
		settings -> realFrequencies = !(key.compare("TRUE"));
	}
	if (parsedData["micro"].find("text_align") != parsedData["micro"].end()) {
		key = parsedData["micro"]["text_align"];
		convertToUpper(key);
		if (!key.compare("LEFT")) {
			settings -> alignTo = 0;
		}
		else if (!key.compare("CENTER")) {
			settings -> alignTo = 1;
		}		
		else if (!key.compare("RIGHT")) {
			settings -> alignTo = 2;
		}
	}
	long maxFontSize = 18;
	long minFontSize = 8;
	if (parsedData["micro"].find("handheld_font_size") != parsedData["micro"].end()) {
		key = parsedData["micro"]["handheld_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> handheldFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> handheldFontSize = maxFontSize;
		else settings -> handheldFontSize = fontsize;	
	}
	if (parsedData["micro"].find("docked_font_size") != parsedData["micro"].end()) {
		key = parsedData["micro"]["docked_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> dockedFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> dockedFontSize = maxFontSize;
		else settings -> dockedFontSize = fontsize;	
	}
	if (parsedData["micro"].find("background_color") != parsedData["micro"].end()) {
		key = parsedData["micro"]["background_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> backgroundColor = temp;
	}
	if (parsedData["micro"].find("cat_color") != parsedData["micro"].end()) {
		key = parsedData["micro"]["cat_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> catColor = temp;
	}
	if (parsedData["micro"].find("text_color") != parsedData["micro"].end()) {
		key = parsedData["micro"]["text_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> textColor = temp;
	}
	if (parsedData["micro"].find("replace_GB_with_RAM_load") != parsedData["micro"].end()) {
		key = parsedData["micro"]["replace_GB_with_RAM_load"];
		convertToUpper(key);
		settings -> showRAMLoad = key.compare("FALSE");
	}
	if (parsedData["micro"].find("show") != parsedData["micro"].end()) {
		key = parsedData["micro"]["show"];
		convertToUpper(key);
		settings -> show = key;
	}
	if (parsedData["micro"].find("layer_height_align") != parsedData["micro"].end()) {
		key = parsedData["micro"]["layer_height_align"];
		convertToUpper(key);
		settings -> setPosBottom = !key.compare("BOTTOM");
	}
}

void GetConfigSettings(FpsCounterSettings* settings) {
	settings -> handheldFontSize = 40;
	settings -> dockedFontSize = 40;
	convertStrToRGBA4444("#1117", &(settings -> backgroundColor));
	convertStrToRGBA4444("#FFFF", &(settings -> textColor));
	settings -> setPos = 0;
	settings -> refreshRate = 31;

	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (parsedData.find("fps-counter") == parsedData.end())
		return;
	long maxFontSize = 150;
	long minFontSize = 8;
	if (parsedData["fps-counter"].find("handheld_font_size") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["handheld_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> handheldFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> handheldFontSize = maxFontSize;
		else settings -> handheldFontSize = fontsize;	
	}
	if (parsedData["fps-counter"].find("docked_font_size") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["docked_font_size"];
		long fontsize = atol(key.c_str());
		if (fontsize < minFontSize)
			settings -> dockedFontSize = minFontSize;
		else if (fontsize > maxFontSize)
			settings -> dockedFontSize = maxFontSize;
		else settings -> dockedFontSize = fontsize;	
	}
	if (parsedData["fps-counter"].find("background_color") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["background_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> backgroundColor = temp;
	}
	if (parsedData["fps-counter"].find("text_color") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["text_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> textColor = temp;
	}
	if (parsedData["fps-counter"].find("layer_width_align") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["layer_width_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos = 1;
		}
		if (!key.compare("RIGHT")) {
			settings -> setPos = 2;
		}
	}
	if (parsedData["fps-counter"].find("layer_height_align") != parsedData["fps-counter"].end()) {
		key = parsedData["fps-counter"]["layer_height_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos += 3;
		}
		if (!key.compare("BOTTOM")) {
			settings -> setPos += 6;
		}
	}
}

void GetConfigSettings(FpsGraphSettings* settings) {
	settings -> setPos = 0;
	convertStrToRGBA4444("#1117", &(settings -> backgroundColor));
	convertStrToRGBA4444("#4444", &(settings -> fpsColor));
	convertStrToRGBA4444("#F77F", &(settings -> borderColor));
	convertStrToRGBA4444("#8888", &(settings -> dashedLineColor));
	convertStrToRGBA4444("#FFFF", &(settings -> maxFPSTextColor));
	convertStrToRGBA4444("#FFFF", &(settings -> minFPSTextColor));
	convertStrToRGBA4444("#FFFF", &(settings -> mainLineColor));
	convertStrToRGBA4444("#F0FF", &(settings -> roundedLineColor));
	convertStrToRGBA4444("#0C0F", &(settings -> perfectLineColor));
	settings -> refreshRate = 31;

	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (parsedData.find("fps-graph") == parsedData.end())
		return;
	if (parsedData["fps-graph"].find("layer_width_align") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["layer_width_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos = 1;
		}
		if (!key.compare("RIGHT")) {
			settings -> setPos = 2;
		}
	}
	if (parsedData["fps-graph"].find("layer_height_align") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["layer_height_align"];
		convertToUpper(key);
		if (!key.compare("CENTER")) {
			settings -> setPos += 3;
		}
		if (!key.compare("BOTTOM")) {
			settings -> setPos += 6;
		}
	}
	if (parsedData["fps-graph"].find("min_fps_text_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["min_fps_text_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> minFPSTextColor = temp;
	}
	if (parsedData["fps-graph"].find("max_fps_text_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["max_fps_text_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> maxFPSTextColor = temp;
	}
	if (parsedData["fps-graph"].find("background_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["background_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> backgroundColor = temp;
	}
	if (parsedData["fps-graph"].find("fps_counter_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["fps_counter_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> fpsColor = temp;
	}
	if (parsedData["fps-graph"].find("border_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["border_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> borderColor = temp;
	}
	if (parsedData["fps-graph"].find("dashed_line_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["dashed_line_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> dashedLineColor = temp;
	}
	if (parsedData["fps-graph"].find("main_line_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["main_line_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> mainLineColor = temp;
	}
	if (parsedData["fps-graph"].find("rounded_line_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["rounded_line_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> roundedLineColor = temp;
	}
	if (parsedData["fps-graph"].find("perfect_line_color") != parsedData["fps-graph"].end()) {
		key = parsedData["fps-graph"]["perfect_line_color"];
		uint16_t temp = 0;
		if (convertStrToRGBA4444(key, &temp))
			settings -> perfectLineColor = temp;
	}
}

void GetConfigSettings(FullSettings* settings) {
	settings -> setPosRight = false;
	settings -> refreshRate = 1;
	settings -> showRealFreqs = true;
	settings -> showDeltas = true;
	settings -> showTargetFreqs = true;

	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (parsedData.find("full") == parsedData.end())
		return;
	if (parsedData["full"].find("refresh_rate") != parsedData["full"].end()) {
		long maxFPS = 60;
		long minFPS = 1;
		
		key = parsedData["full"]["refresh_rate"];
		long rate = atol(key.c_str());
		if (rate < minFPS) {
			settings -> refreshRate = minFPS;
		}
		else if (rate > maxFPS)
			settings -> refreshRate = maxFPS;
		else settings -> refreshRate = rate;	
	}
	if (parsedData["full"].find("layer_width_align") != parsedData["full"].end()) {
		key = parsedData["full"]["layer_width_align"];
		convertToUpper(key);
		settings -> setPosRight = !key.compare("RIGHT");
	}
	if (parsedData["full"].find("show_real_freqs") != parsedData["full"].end()) {
		key = parsedData["full"]["show_real_freqs"];
		convertToUpper(key);
		settings -> showRealFreqs = key.compare("FALSE");
	}
	if (parsedData["full"].find("show_deltas") != parsedData["full"].end()) {
		key = parsedData["full"]["show_deltas"];
		convertToUpper(key);
		settings -> showDeltas = key.compare("FALSE");
	}
	if (parsedData["full"].find("show_target_freqs") != parsedData["full"].end()) {
		key = parsedData["full"]["show_target_freqs"];
		convertToUpper(key);
		settings -> showTargetFreqs = key.compare("FALSE");
	}
}
