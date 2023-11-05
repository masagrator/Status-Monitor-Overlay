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
Thread t5;
Thread t6;
Thread t7;
uint64_t systemtickfrequency = 19200000;
bool threadexit = false;
bool threadexit2 = false;
uint64_t refreshrate = 1;
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
int32_t realCPU_Hz = 0;
int32_t realGPU_Hz = 0;
int32_t realRAM_Hz = 0;

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

//Check for input outside of FPS limitations
void CheckButtons(void*) {
	static uint64_t kHeld = padGetButtons(&pad);
	while (!threadexit) {
		padUpdate(&pad);
		kHeld = padGetButtons(&pad);
		if ((kHeld & KEY_ZR) && (kHeld & KEY_R)) {
			if (kHeld & KEY_DDOWN) {
				TeslaFPS = 1;
				refreshrate = 1;
				systemtickfrequency = 19200000;
			}
			else if (kHeld & KEY_DUP) {
				TeslaFPS = 5;
				refreshrate = 5;
				systemtickfrequency = 3840000;
			}
		}
		svcSleepThread(100'000'000);
	}
}

void BatteryChecker(void*) {
	if (R_SUCCEEDED(psmCheck)){
		u16 data = 0;
		float tempV = 0;
		float tempA = 0;
		size_t ArraySize = 10;
		size_t CommonPowerAvgHistorySize = 3; // last 3 min history
		size_t TmpPowerHistoryArraySize = 120; // last 60 sec history
		float readingsAmp[ArraySize] = {0};
		float readingsVolt[ArraySize] = {0};
		std::vector<float> commonAvgPowerHistory; // common avg history
		float tmpPowerHistory[TmpPowerHistoryArraySize] = {0};

		if (Max17050ReadReg(MAX17050_Current, &data)) {
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			for (size_t i = 0; i < ArraySize; i++) {
				readingsAmp[i] = tempA;
			}
		}
		svcSleepThread(1000000);
		if (Max17050ReadReg(MAX17050_VCELL, &data)) {
			tempV = 0.625 * (data >> 3);
			for (size_t i = 0; i < ArraySize; i++) {
				readingsVolt[i] = tempV;
			}
		}
		svcSleepThread(1000000);
		if (!actualFullBatCapacity && Max17050ReadReg(MAX17050_FullCAP, &data)) {
			actualFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
		}
		svcSleepThread(1000000);
		if (!designedFullBatCapacity && Max17050ReadReg(MAX17050_DesignCap, &data)) {
			designedFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
		}
		svcSleepThread(1000000);
		size_t i = 0;
		size_t powerHistoryIteration = 0;
		int tempChargerType = 0;
		if (!Max17050ReadReg(MAX17050_AvgCurrent, &data) && (s16)data > 0) {
			float tmpCurrentAvg = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			while (powerHistoryIteration < 20)
				tmpPowerHistory[powerHistoryIteration++] = tmpCurrentAvg;
		}

		while (!threadexit) {
			svcSleepThread(1000000);
			psmGetBatteryChargeInfoFields(psmService, &_batteryChargeInfoFields);
			// Calculation is based on Hekate's max17050.c
			// Source: https://github.com/CTCaer/hekate/blob/master/bdk/power/max17050.c
			if (!Max17050ReadReg(MAX17050_Current, &data))
				continue;
			svcSleepThread(1000000);
			tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
			if (!Max17050ReadReg(MAX17050_VCELL, &data))
				continue;
			svcSleepThread(1000000);
			tempV = 0.625 * (data >> 3);

			readingsAmp[i] = tempA;
			readingsVolt[i] = tempV;
			if (i+1 < ArraySize) {
				i++;
			}
			else i = 0;
			
			float batCurrent = readingsAmp[0];
			float batVoltage = readingsVolt[0];
			float batPowerAvg = (readingsAmp[0] * readingsVolt[0]) / 1'000;
			for (size_t x = 1; x < ArraySize; x++) {
				batCurrent += readingsAmp[x];
				batVoltage += readingsVolt[x];
				batPowerAvg += (readingsAmp[x] * readingsVolt[x]) / 1'000;
			}
			float actualCapacity = actualFullBatCapacity / 100 * (float)_batteryChargeInfoFields.RawBatteryCharge / 1000;
			batCurrent /= ArraySize;
			batVoltage /= ArraySize;
			batCurrentAvg = batCurrent;
			batVoltageAvg = batVoltage;
			batPowerAvg /= ArraySize * 1000;
			PowerConsumption = batPowerAvg;
			bool chargerTypeDifferent = (tempChargerType != _batteryChargeInfoFields.ChargerType);
			if (hosversionAtLeast(17,0,0)) {
				chargerTypeDifferent = (tempChargerType != ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields) -> ChargerType);
			}
			if (chargerTypeDifferent) {
				powerHistoryIteration = 0;
				batTimeEstimate = -1;
				tempChargerType = _batteryChargeInfoFields.ChargerType;
				if (hosversionAtLeast(17,0,0)) {
					tempChargerType = ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields) -> ChargerType;
				}
				commonAvgPowerHistory.clear();
				commonAvgPowerHistory.shrink_to_fit();
			}
			else if (batCurrentAvg < 0) {
				tmpPowerHistory[powerHistoryIteration++] = batCurrentAvg; // add currentAvg to tmp array
				if (powerHistoryIteration == TmpPowerHistoryArraySize) {
					if (commonAvgPowerHistory.size() == CommonPowerAvgHistorySize) {
						commonAvgPowerHistory.erase(commonAvgPowerHistory.begin());
					}
					float tmpPowerSum = std::accumulate(tmpPowerHistory, tmpPowerHistory+TmpPowerHistoryArraySize, 0);
					commonAvgPowerHistory.push_back(tmpPowerSum / TmpPowerHistoryArraySize);
					float commonPowerSum = std::accumulate(commonAvgPowerHistory.begin(), commonAvgPowerHistory.end(), 0);
					float commonAvg = -commonPowerSum / commonAvgPowerHistory.size();
					batTimeEstimate = (int)(actualCapacity / (commonAvg / 60));
					if (batTimeEstimate > (99*60)+59)
						batTimeEstimate = (99*60)+59;
					powerHistoryIteration = 0;
				}
				else if (commonAvgPowerHistory.size() == 0 && powerHistoryIteration < TmpPowerHistoryArraySize) {
					float PowerSum = std::accumulate(tmpPowerHistory, tmpPowerHistory+powerHistoryIteration, 0);
					float commonAvg = -PowerSum / powerHistoryIteration;
					batTimeEstimate = (int)(actualCapacity / (commonAvg / 60));
					if (batTimeEstimate > (99*60)+59)
						batTimeEstimate = (99*60)+59;
				}
			}
			else {
				powerHistoryIteration = 0;
				batTimeEstimate = -1;
			}
			svcSleepThread(499'000'000);
		}
		_batteryChargeInfoFields = {0};
		commonAvgPowerHistory.clear();
		commonAvgPowerHistory.shrink_to_fit();
	}
}

void StartBatteryThread() {
	threadCreate(&t7, BatteryChecker, NULL, NULL, 0x4000, 0x3F, 3);
	threadStart(&t7);
}

//Stuff that doesn't need multithreading
void Misc(void*) {
	while (!threadexit) {
		
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
		svcSleepThread(100'000'000);
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
void CheckCore0(void*) {
	while (!threadexit) {
		static uint64_t idletick_a0 = 0;
		static uint64_t idletick_b0 = 0;
		svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		svcSleepThread(1'000'000'000 / refreshrate);
		svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		idletick0 = idletick_a0 - idletick_b0;
	}
}

void CheckCore1(void*) {
	while (!threadexit) {
		static uint64_t idletick_a1 = 0;
		static uint64_t idletick_b1 = 0;
		svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		svcSleepThread(1'000'000'000 / refreshrate);
		svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		idletick1 = idletick_a1 - idletick_b1;
	}
}

void CheckCore2(void*) {
	while (!threadexit) {
		static uint64_t idletick_a2 = 0;
		static uint64_t idletick_b2 = 0;
		svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		svcSleepThread(1'000'000'000 / refreshrate);
		svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		idletick2 = idletick_a2 - idletick_b2;
	}
}

void CheckCore3(void*) {
	while (!threadexit) {
		static uint64_t idletick_a3 = 0;
		static uint64_t idletick_b3 = 0;
		svcGetInfo(&idletick_b3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
		svcSleepThread(1'000'000'000 / refreshrate);
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
	threadCreate(&t5, CheckButtons, NULL, NULL, 0x1000, 0x3F, -2);
	if (SaltySD) {
		//Assign NX-FPS to default core
		threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
	}
				
	threadStart(&t0);
	threadStart(&t1);
	threadStart(&t2);
	threadStart(&t3);
	threadStart(&t4);
	threadStart(&t5);
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
	threadWaitForExit(&t5);
	threadWaitForExit(&t6);
	threadWaitForExit(&t7);
	threadClose(&t0);
	threadClose(&t1);
	threadClose(&t2);
	threadClose(&t3);
	threadClose(&t4);
	threadClose(&t5);
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
		svcSleepThread(1'000'000'000 / refreshrate);
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

void formatButtonCombination(std::string& line) {
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
}


// Base class with virtual function
class ButtonMapper {
public:
	virtual std::list<HidNpadButton> MapButtons(const std::string& buttonCombo) = 0;
};

// Derived class implementing the virtual function
class ButtonMapperImpl : public ButtonMapper {
public:
	std::list<HidNpadButton> MapButtons(const std::string& buttonCombo) override {
		std::map<std::string, HidNpadButton> buttonMap = {
			{"A", static_cast<HidNpadButton>(HidNpadButton_A)},
			{"B", static_cast<HidNpadButton>(HidNpadButton_B)},
			{"X", static_cast<HidNpadButton>(HidNpadButton_X)},
			{"Y", static_cast<HidNpadButton>(HidNpadButton_Y)},
			{"L", static_cast<HidNpadButton>(HidNpadButton_L)},
			{"R", static_cast<HidNpadButton>(HidNpadButton_R)},
			{"ZL", static_cast<HidNpadButton>(HidNpadButton_ZL)},
			{"ZR", static_cast<HidNpadButton>(HidNpadButton_ZR)},
			{"PLUS", static_cast<HidNpadButton>(HidNpadButton_Plus)},
			{"MINUS", static_cast<HidNpadButton>(HidNpadButton_Minus)},
			{"DUP", static_cast<HidNpadButton>(HidNpadButton_Up)},
			{"DDOWN", static_cast<HidNpadButton>(HidNpadButton_Down)},
			{"DLEFT", static_cast<HidNpadButton>(HidNpadButton_Left)},
			{"DRIGHT", static_cast<HidNpadButton>(HidNpadButton_Right)},
			{"SL", static_cast<HidNpadButton>(HidNpadButton_AnySL)},
			{"SR", static_cast<HidNpadButton>(HidNpadButton_AnySR)},
			{"LSTICK", static_cast<HidNpadButton>(HidNpadButton_StickL)},
			{"RSTICK", static_cast<HidNpadButton>(HidNpadButton_StickR)},
			{"UP", static_cast<HidNpadButton>(HidNpadButton_Up | HidNpadButton_StickLUp | HidNpadButton_StickRUp)},
			{"DOWN", static_cast<HidNpadButton>(HidNpadButton_Down | HidNpadButton_StickLDown | HidNpadButton_StickRDown)},
			{"LEFT", static_cast<HidNpadButton>(HidNpadButton_Left | HidNpadButton_StickLLeft | HidNpadButton_StickRLeft)},
			{"RIGHT", static_cast<HidNpadButton>(HidNpadButton_Right | HidNpadButton_StickLRight | HidNpadButton_StickRRight)}
		};

		std::list<HidNpadButton> mappedButtons;
		std::string comboCopy = buttonCombo;  // Make a copy of buttonCombo

		std::string delimiter = "+";
		size_t pos = 0;
		std::string button;
		size_t max_delimiters = 4;
		while ((pos = comboCopy.find(delimiter)) != std::string::npos) {
			button = comboCopy.substr(0, pos);
			if (buttonMap.find(button) != buttonMap.end()) {
				mappedButtons.push_back(buttonMap[button]);
			}
			comboCopy.erase(0, pos + delimiter.length());
			if(!--max_delimiters) {
				return mappedButtons;
			}
		}
		if (buttonMap.find(comboCopy) != buttonMap.end()) {
			mappedButtons.push_back(buttonMap[comboCopy]);
		}
		return mappedButtons;
	}
};

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
			
		// Read the contents of the INI file
		char* fileData = new char[fileSize + 1];
		fread(fileData, sizeof(char), fileSize, configFileIn);
		fileData[fileSize] = '\0';  // Add null-terminator to create a C-string
		fclose(configFileIn);
			
		// Parse the INI data
		std::string fileDataString(fileData, fileSize);
		parsedData = tsl::hlp::ini::parseIni(fileDataString);
		delete[] fileData;
		
		// Access and use the parsed data as needed
		// For example, print the value of a specific section and key
		if (parsedData.find("status-monitor") != parsedData.end() &&
			parsedData["status-monitor"].find("key_combo") != parsedData["status-monitor"].end()) {
			keyCombo = parsedData["status-monitor"]["key_combo"]; // load keyCombo variable
			removeSpaces(keyCombo); // format combo
			convertToUpper(keyCombo);
		} else {
			readExternalCombo = true;
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

bool renderRealFreqs(bool Micro) {
	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return false;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);

	std::string key;
	if (Micro) {
		if (parsedData.find("micro") == parsedData.end())
			return false;
		if (parsedData["micro"].find("real_freqs") == parsedData["micro"].end())
			return false;
		key = parsedData["micro"]["real_freqs"];
	}
	else {
		if (parsedData.find("mini") == parsedData.end())
			return false;
		if (parsedData["mini"].find("real_freqs") == parsedData["mini"].end())
			return false;
		key = parsedData["mini"]["real_freqs"];
	}
	convertToUpper(key);
	return !(key.compare("TRUE"));
}

size_t getFontSize(bool Micro) {
	size_t defaultFontSize = 15;
	if (Micro) {
		defaultFontSize = 18;
	}
	FILE* configFileIn = fopen("sdmc:/config/status-monitor/config.ini", "r");
	if (!configFileIn)
		return false;
	fseek(configFileIn, 0, SEEK_END);
	long fileSize = ftell(configFileIn);
	rewind(configFileIn);

	std::string fileDataString(fileSize, '\0');
	fread(&fileDataString[0], sizeof(char), fileSize, configFileIn);
	fclose(configFileIn);
	
	auto parsedData = tsl::hlp::ini::parseIni(fileDataString);
	
	std::string key;
	if (Micro) {
		if (parsedData.find("micro") == parsedData.end())
			return defaultFontSize;
		if (parsedData["micro"].find("font_size") == parsedData["micro"].end())
			return defaultFontSize;
		key = parsedData["micro"]["font_size"];
	}
	else {
		if (parsedData.find("mini") == parsedData.end())
			return defaultFontSize;
		if (parsedData["mini"].find("font_size") == parsedData["mini"].end())
			return defaultFontSize;
		key = parsedData["mini"]["font_size"];
	}
	long fontsize = atol(key.c_str());

	long maxFontSize = 22;
	if (Micro)
		maxFontSize = 18;
	long minFontSize = 8;
	if (fontsize < minFontSize)
		return minFontSize;
	if (fontsize > maxFontSize)
		return maxFontSize;
	return fontsize;
}