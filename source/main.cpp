#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "SaltyNX.h"

#include "Battery.hpp"
#include "audsnoop.h"
#include "Misc.hpp"
#include "i2c.h"
#include "max17050.h"

#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715
#define FieldDescriptor uint32_t

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
std::string filepath = "sdmc:/switch/.overlays/Status-Monitor-Overlay.ovl";

//Misc2
NvChannel nvdecChannel;

//Mini mode
char Variables[768];

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
Result nifmCheck = 1;

//Wi-Fi
NifmInternetConnectionType NifmConnectionType = (NifmInternetConnectionType)-1;
NifmInternetConnectionStatus NifmConnectionStatus = (NifmInternetConnectionStatus)-1;
bool Nifm_showpass = false;
Result Nifm_internet_rc = -1;
Result Nifm_profile_rc = -1;
NifmNetworkProfileData_new Nifm_profile = {0};
char Nifm_pass[96];

//NVDEC
uint32_t NVDEC_Hz = 0;
char NVDEC_Hz_c[32];

//DSP
uint32_t DSP_Load_u = -1;
char DSP_Load_c[16];

//Battery
Service* psmService = 0;
BatteryChargeInfoFields _batteryChargeInfoFields = {0};
char Battery_c[320];
char BatteryDraw_c[64];
float batCurrentAvg = 0;
float batVoltageAvg = 0;

//Temperatures
int32_t SOC_temperatureC = 0;
int32_t PCB_temperatureC = 0;
int32_t skin_temperaturemiliC = 0;
char SoCPCB_temperature_c[64];
char skin_temperature_c[32];

//CPU Usage
uint64_t idletick0 = systemtickfrequency;
uint64_t idletick1 = systemtickfrequency;
uint64_t idletick2 = systemtickfrequency;
uint64_t idletick3 = systemtickfrequency;
char CPU_Usage0[32];
char CPU_Usage1[32];
char CPU_Usage2[32];
char CPU_Usage3[32];
char CPU_compressed_c[160];

//Frequency
///CPU
uint32_t CPU_Hz = 0;
char CPU_Hz_c[32];
///GPU
uint32_t GPU_Hz = 0;
char GPU_Hz_c[32];
///RAM
uint32_t RAM_Hz = 0;
char RAM_Hz_c[32];

//RAM Size
char RAM_all_c[64];
char RAM_application_c[64];
char RAM_applet_c[64];
char RAM_system_c[64];
char RAM_systemunsafe_c[64];
char RAM_compressed_c[320];
char RAM_var_compressed_c[320];
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
char Rotation_SpeedLevel_c[64];

//GPU Usage
FieldDescriptor fd = 0;
uint32_t GPU_Load_u = 0;
char GPU_Load_c[32];

//NX-FPS
bool GameRunning = false;
bool check = true;
bool SaltySD = false;
uintptr_t FPSaddress = 0;
uintptr_t FPSavgaddress = 0;
uint64_t PID = 0;
uint32_t FPS = 0xFE;
float FPSavg = 254;
char FPS_c[32];
char FPSavg_c[32];
char FPS_compressed_c[64];
char FPS_var_compressed_c[64];
SharedMemory _sharedmemory = {};
bool SharedMemoryUsed = false;
uint32_t* MAGIC_shared = 0;
uint8_t* FPS_shared = 0;
float* FPSavg_shared = 0;
bool* pluginActive = 0;
Handle remoteSharedMemory = 1;

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
		float temp = 0;
		if (Max17050ReadReg(MAX17050_Current, &data)) {
			batCurrentAvg = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
		}
		if (Max17050ReadReg(MAX17050_VCELL, &data)) {
			batVoltageAvg = 0.625 * (data >> 3);
		}
		while (!threadexit) {
			psmGetBatteryChargeInfoFields(psmService, &_batteryChargeInfoFields);
			// Calculation is based on Hekate's max17050.c
			// Source: https://github.com/CTCaer/hekate/blob/master/bdk/power/max17050.c
			if (Max17050ReadReg(MAX17050_Current, &data)) {
				temp = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
				batCurrentAvg = ((4 * batCurrentAvg) + temp) / 5;
			}
			if (Max17050ReadReg(MAX17050_VCELL, &data)) {
				temp = 0.625 * (data >> 3);
				batVoltageAvg = ((4 * batVoltageAvg) + temp) / 5;
			}
			svcSleepThread(1'000'000'000);
		}
		_batteryChargeInfoFields = {0};
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
		
		//Temperatures
		if (R_SUCCEEDED(tsCheck)) {
			if (hosversionAtLeast(14,0,0)) {
				tsGetTemperature(TsLocation_External, &SOC_temperatureC);
				tsGetTemperature(TsLocation_Internal, &PCB_temperatureC);
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
				FPSavg = *FPSavg_shared;
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

		//NVDEC clock rate
		if (R_SUCCEEDED(nvdecCheck)) getNvChannelClockRate(&nvdecChannel, 0x75, &NVDEC_Hz);

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
	threadCreate(&t0, CheckCore0, NULL, NULL, 0x100, 0x10, 0);
	threadCreate(&t1, CheckCore1, NULL, NULL, 0x100, 0x10, 1);
	threadCreate(&t2, CheckCore2, NULL, NULL, 0x100, 0x10, 2);
	threadCreate(&t3, CheckCore3, NULL, NULL, 0x100, 0x10, 3);
	threadCreate(&t4, Misc, NULL, NULL, 0x100, 0x3F, -2);
	threadCreate(&t5, CheckButtons, NULL, NULL, 0x400, 0x3F, -2);
	threadStart(&t0);
	threadStart(&t1);
	threadStart(&t2);
	threadStart(&t3);
	threadStart(&t4);
	threadStart(&t5);
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
				FPSavg = *FPSavg_shared;
			}
		}
		else FPSavg = 254;
		svcSleepThread(1'000'000'000 / refreshrate);
	}
}

void StartFPSCounterThread() {
	threadCreate(&t0, FPSCounter, NULL, NULL, 0x1000, 0x3F, 3);
	threadStart(&t0);
}

void EndFPSCounterThread() {
	threadexit = true;
	threadWaitForExit(&t0);
	threadClose(&t0);
	threadexit = false;
}

//FPS Counter mode
class com_FPS : public tsl::Gui {
public:
    com_FPS() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
				static uint8_t avg = 0;
				if (FPSavg >= 100) avg = 46;
				else if (FPSavg >= 10) avg = 23;
				else avg = 0;
				renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth - 370 + avg, 50, a(0x7111));
				renderer->drawString(FPSavg_c, false, 5, 40, 40, renderer->a(0xFFFF));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		///FPS
		snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f", FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			EndFPSCounterThread();
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Full mode
class FullOverlay : public tsl::Gui {
public:
    FullOverlay() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			//Print strings
			///CPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {
				renderer->drawString("CPU Usage:", false, 20, 120, 20, renderer->a(0xFFFF));
				renderer->drawString(CPU_Hz_c, false, 20, 155, 15, renderer->a(0xFFFF));
				renderer->drawString(CPU_compressed_c, false, 20, 185, 15, renderer->a(0xFFFF));
			}
			
			///GPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(nvCheck)) {
				
				renderer->drawString("GPU Usage:", false, 20, 285, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) renderer->drawString(GPU_Hz_c, false, 20, 320, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(nvCheck)) renderer->drawString(GPU_Load_c, false, 20, 335, 15, renderer->a(0xFFFF));
				
			}
			
			///RAM
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(Hinted)) {
				
				renderer->drawString("RAM Usage:", false, 20, 375, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) renderer->drawString(RAM_Hz_c, false, 20, 410, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(Hinted)) {
					renderer->drawString(RAM_compressed_c, false, 20, 440, 15, renderer->a(0xFFFF));
					renderer->drawString(RAM_var_compressed_c, false, 140, 440, 15, renderer->a(0xFFFF));
				}
			}
			
			///Thermal
			if (R_SUCCEEDED(tsCheck) || R_SUCCEEDED(tcCheck) || R_SUCCEEDED(fanCheck)) {
				renderer->drawString("Board:", false, 20, 540, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString(BatteryDraw_c, false, 20, 575, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString("Temperatures: SoC\n\t\t\t\t\t\t\t PCB\n\t\t\t\t\t\t\t Skin", false, 20, 590, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString(SoCPCB_temperature_c, false, 170, 590, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(fanCheck)) renderer->drawString(Rotation_SpeedLevel_c, false, 20, 635, 15, renderer->a(0xFFFF));
			}
			
			///FPS
			if (GameRunning) {
				renderer->drawString(FPS_compressed_c, false, 235, 120, 20, renderer->a(0xFFFF));
				renderer->drawString(FPS_var_compressed_c, false, 295, 120, 20, renderer->a(0xFFFF));
			}
			
			if (refreshrate == 5) renderer->drawString("Hold Left Stick & Right Stick to Exit\nHold ZR + R + D-Pad Down to slow down refresh", false, 20, 675, 15, renderer->a(0xFFFF));
			else if (refreshrate == 1) renderer->drawString("Hold Left Stick & Right Stick to Exit\nHold ZR + R + D-Pad Up to speed up refresh", false, 20, 675, 15, renderer->a(0xFFFF));
		
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		if (TeslaFPS == 60) TeslaFPS = 1;
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz", (float)CPU_Hz / 1000000);
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "Core #0: %.2f%s", ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "Core #1: %.2f%s", ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "Core #2: %.2f%s", ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "Core #3: %.2f%s", ((double)systemtickfrequency - (double)idletick3) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "%s\n%s\n%s\n%s", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3);
		
		///GPU
		snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz", (float)GPU_Hz / 1000000);
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "Load: %.1f%s", (float)GPU_Load_u / 10, "%");
		
		///RAM
		snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz", (float)RAM_Hz / 1000000);
		float RAM_Total_application_f = (float)RAM_Total_application_u / 1024 / 1024;
		float RAM_Total_applet_f = (float)RAM_Total_applet_u / 1024 / 1024;
		float RAM_Total_system_f = (float)RAM_Total_system_u / 1024 / 1024;
		float RAM_Total_systemunsafe_f = (float)RAM_Total_systemunsafe_u / 1024 / 1024;
		float RAM_Total_all_f = RAM_Total_application_f + RAM_Total_applet_f + RAM_Total_system_f + RAM_Total_systemunsafe_f;
		float RAM_Used_application_f = (float)RAM_Used_application_u / 1024 / 1024;
		float RAM_Used_applet_f = (float)RAM_Used_applet_u / 1024 / 1024;
		float RAM_Used_system_f = (float)RAM_Used_system_u / 1024 / 1024;
		float RAM_Used_systemunsafe_f = (float)RAM_Used_systemunsafe_u / 1024 / 1024;
		float RAM_Used_all_f = RAM_Used_application_f + RAM_Used_applet_f + RAM_Used_system_f + RAM_Used_systemunsafe_f;
		snprintf(RAM_all_c, sizeof RAM_all_c, "Total:");
		snprintf(RAM_application_c, sizeof RAM_application_c, "Application:");
		snprintf(RAM_applet_c, sizeof RAM_applet_c, "Applet:");
		snprintf(RAM_system_c, sizeof RAM_system_c, "System:");
		snprintf(RAM_systemunsafe_c, sizeof RAM_systemunsafe_c, "System Unsafe:");
		snprintf(RAM_compressed_c, sizeof RAM_compressed_c, "%s\n%s\n%s\n%s\n%s", RAM_all_c, RAM_application_c, RAM_applet_c, RAM_system_c, RAM_systemunsafe_c);
		snprintf(RAM_all_c, sizeof RAM_all_c, "%4.2f / %4.2f MB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(RAM_application_c, sizeof RAM_application_c, "%4.2f / %4.2f MB", RAM_Used_application_f, RAM_Total_application_f);
		snprintf(RAM_applet_c, sizeof RAM_applet_c, "%4.2f / %4.2f MB", RAM_Used_applet_f, RAM_Total_applet_f);
		snprintf(RAM_system_c, sizeof RAM_system_c, "%4.2f / %4.2f MB", RAM_Used_system_f, RAM_Total_system_f);
		snprintf(RAM_systemunsafe_c, sizeof RAM_systemunsafe_c, "%4.2f / %4.2f MB", RAM_Used_systemunsafe_f, RAM_Total_systemunsafe_f);
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "%s\n%s\n%s\n%s\n%s", RAM_all_c, RAM_application_c, RAM_applet_c, RAM_system_c, RAM_systemunsafe_c);
		
		///Thermal
		float PowerConsumption = ((batCurrentAvg / 1000) * (batVoltageAvg / 1000));
		snprintf(BatteryDraw_c, sizeof BatteryDraw_c, "Battery Power Flow: %+.2fW", PowerConsumption);
		if (hosversionAtLeast(14,0,0)) {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%2d \u00B0C\n%2d \u00B0C\n%2.2f \u00B0C", SOC_temperatureC, PCB_temperatureC, (float)skin_temperaturemiliC / 1000);
		}
		else 
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%2.2f \u00B0C\n%2.2f\u00B0C\n%2.2f \u00B0C", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "Fan Rotation Level:\t%2.2f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_c, sizeof FPS_c, "PFPS:"); //Pushed Frames Per Second
		snprintf(FPSavg_c, sizeof FPSavg_c, "FPS:"); //Frames Per Second calculated from averaged frametime 
		snprintf(FPS_compressed_c, sizeof FPS_compressed_c, "%s\n%s", FPS_c, FPSavg_c);
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%u\n%2.2f", FPS, FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			CloseThreads();
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Mini mode
class MiniOverlay : public tsl::Gui {
public:
    MiniOverlay() { }

    virtual tsl::elm::Element* createUI() override {

		auto rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			if (!GameRunning) renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth - 150, 95, a(0x7111));
			else renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth - 150, 125, a(0x7111));
			
			//Print strings
			///CPU
			if (GameRunning) renderer->drawString("CPU\nGPU\nRAM\nTEMP\nFAN\nDRAW\nPFPS\nFPS", false, 0, 15, 15, renderer->a(0xFFFF));
			else renderer->drawString("CPU\nGPU\nRAM\nTEMP\nFAN\nDRAW", false, 0, 15, 15, renderer->a(0xFFFF));
			
			///GPU
			renderer->drawString(Variables, false, 60, 15, 15, renderer->a(0xFFFF));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		if (TeslaFPS == 60) TeslaFPS = 1;
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		double percent = ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick3) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "%.0f%s", percent, "%");
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "[%s,%s,%s,%s]@%.1f", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3, (float)CPU_Hz / 1000000);
		
		///GPU
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "%.1f%s@%.1f", (float)GPU_Load_u / 10, "%", (float)GPU_Hz / 1000000);
		
		///RAM
		float RAM_Total_application_f = (float)RAM_Total_application_u / 1024 / 1024;
		float RAM_Total_applet_f = (float)RAM_Total_applet_u / 1024 / 1024;
		float RAM_Total_system_f = (float)RAM_Total_system_u / 1024 / 1024;
		float RAM_Total_systemunsafe_f = (float)RAM_Total_systemunsafe_u / 1024 / 1024;
		float RAM_Total_all_f = RAM_Total_application_f + RAM_Total_applet_f + RAM_Total_system_f + RAM_Total_systemunsafe_f;
		float RAM_Used_application_f = (float)RAM_Used_application_u / 1024 / 1024;
		float RAM_Used_applet_f = (float)RAM_Used_applet_u / 1024 / 1024;
		float RAM_Used_system_f = (float)RAM_Used_system_u / 1024 / 1024;
		float RAM_Used_systemunsafe_f = (float)RAM_Used_systemunsafe_u / 1024 / 1024;
		float RAM_Used_all_f = RAM_Used_application_f + RAM_Used_applet_f + RAM_Used_system_f + RAM_Used_systemunsafe_f;
		snprintf(RAM_all_c, sizeof RAM_all_c, "%.0f/%.0fMB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "%s@%.1f", RAM_all_c, (float)RAM_Hz / 1000000);
		
		///Thermal
		float PowerConsumption = ((batCurrentAvg / 1000) * (batVoltageAvg / 1000));
		snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%0.2fW", PowerConsumption);
		if (hosversionAtLeast(14,0,0))
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2d\u00B0C/%2d\u00B0C/%2.1f\u00B0C", SOC_temperatureC, PCB_temperatureC, (float)skin_temperaturemiliC / 1000);
		else
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f\u00B0C/%2.1f\u00B0C/%2.1f\u00B0C", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.2f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_c, sizeof FPS_c, "PFPS:"); //Pushed Frames Per Second
		snprintf(FPSavg_c, sizeof FPSavg_c, "FPS:"); //Frames Per Second calculated from averaged frametime 
		snprintf(FPS_compressed_c, sizeof FPS_compressed_c, "%s\n%s", FPS_c, FPSavg_c);
		snprintf(FPS_var_compressed_c, sizeof FPS_compressed_c, "%u\n%2.2f", FPS, FPSavg);

		if (GameRunning) snprintf(Variables, sizeof Variables, "%s\n%s\n%s\n%s\n%s\n%s\n%s", CPU_compressed_c, GPU_Load_c, RAM_var_compressed_c, skin_temperature_c, Rotation_SpeedLevel_c, SoCPCB_temperature_c, FPS_var_compressed_c);
		else snprintf(Variables, sizeof Variables, "%s\n%s\n%s\n%s\n%s\n%s", CPU_compressed_c, GPU_Load_c, RAM_var_compressed_c, skin_temperature_c, Rotation_SpeedLevel_c, SoCPCB_temperature_c);

	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			CloseThreads();
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Mini mode
class MicroOverlay : public tsl::Gui {
public:
    MicroOverlay() { }

    virtual tsl::elm::Element* createUI() override {

		auto rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			if (!GameRunning) {
				uint32_t size = 18;
				uint32_t offset1 = 0;
				uint32_t offset2 = offset1 + 355;
				uint32_t offset3 = offset2 + 210;
				uint32_t offset4 = offset3 + 275;
				uint32_t offset5 = offset4 + 320;
				renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth, 22, a(0x7111));
				renderer->drawString("CPU", false, offset1, size, size, renderer->a(0xFCCF));
				renderer->drawString("GPU", false, offset2, size, size, renderer->a(0xFCCF));
				renderer->drawString("RAM", false, offset3, size, size, renderer->a(0xFCCF));
				renderer->drawString("BRD", false, offset4, size, size, renderer->a(0xFCCF));
				renderer->drawString("FAN", false, offset5, size, size, renderer->a(0xFCCF));
				renderer->drawString(CPU_compressed_c, false, offset1+42, size, size, renderer->a(0xFFFF));
				renderer->drawString(GPU_Load_c, false, offset2+45, size, size, renderer->a(0xFFFF));
				renderer->drawString(RAM_var_compressed_c, false, offset3+47, size, size, renderer->a(0xFFFF));
				renderer->drawString(skin_temperature_c, false, offset4+45, size, size, renderer->a(0xFFFF));
				renderer->drawString(Rotation_SpeedLevel_c, false, offset5+43, size, size, renderer->a(0xFFFF));
			}
			else {
				uint32_t size = 18;
				uint32_t offset1 = 0;
				uint32_t offset2 = offset1 + 355;
				uint32_t offset3 = offset2 + 200;
				uint32_t offset4 = offset3 + 265;
				uint32_t offset5 = offset4 + 245;
				uint32_t offset6 = offset5 + 130;
				renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth, 22, a(0x7111));
				renderer->drawString("CPU", false, offset1, size, size, renderer->a(0xFCCF));
				renderer->drawString("GPU", false, offset2, size, size, renderer->a(0xFCCF));
				renderer->drawString("RAM", false, offset3, size, size, renderer->a(0xFCCF));
				renderer->drawString("BRD", false, offset4, size, size, renderer->a(0xFCCF));
				renderer->drawString("FAN", false, offset5, size, size, renderer->a(0xFCCF));
				renderer->drawString("FPS", false, offset6, size, size, renderer->a(0xFCCF));
				renderer->drawString(CPU_compressed_c, false, offset1+42, size, size, renderer->a(0xFFFF));
				renderer->drawString(GPU_Load_c, false, offset2+45, size, size, renderer->a(0xFFFF));
				renderer->drawString(RAM_var_compressed_c, false, offset3+47, size, size, renderer->a(0xFFFF));
				renderer->drawString(skin_temperature_c, false, offset4+45, size, size, renderer->a(0xFFFF));
				renderer->drawString(Rotation_SpeedLevel_c, false, offset5+43, size, size, renderer->a(0xFFFF));
				renderer->drawString(FPS_var_compressed_c, false, offset6+40, size, size, renderer->a(0xFFFF));
			}
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		if (TeslaFPS == 60) {
			TeslaFPS = 1;
			tsl::hlp::requestForeground(false);
		}
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		double percent = ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick3) / (double)systemtickfrequency * 100;
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "%.0f%s", percent, "%");
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "[%s,%s,%s,%s]@%.1f", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3, (float)CPU_Hz / 1000000);
		
		///GPU
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "%.1f%s@%.1f", (float)GPU_Load_u / 10, "%", (float)GPU_Hz / 1000000);
		
		///RAM
		float RAM_Total_application_f = (float)RAM_Total_application_u / 1024 / 1024;
		float RAM_Total_applet_f = (float)RAM_Total_applet_u / 1024 / 1024;
		float RAM_Total_system_f = (float)RAM_Total_system_u / 1024 / 1024;
		float RAM_Total_systemunsafe_f = (float)RAM_Total_systemunsafe_u / 1024 / 1024;
		float RAM_Total_all_f = RAM_Total_application_f + RAM_Total_applet_f + RAM_Total_system_f + RAM_Total_systemunsafe_f;
		float RAM_Used_application_f = (float)RAM_Used_application_u / 1024 / 1024;
		float RAM_Used_applet_f = (float)RAM_Used_applet_u / 1024 / 1024;
		float RAM_Used_system_f = (float)RAM_Used_system_u / 1024 / 1024;
		float RAM_Used_systemunsafe_f = (float)RAM_Used_systemunsafe_u / 1024 / 1024;
		float RAM_Used_all_f = RAM_Used_application_f + RAM_Used_applet_f + RAM_Used_system_f + RAM_Used_systemunsafe_f;
		snprintf(RAM_all_c, sizeof RAM_all_c, "%.0f/%.0fMB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "%s@%.1f", RAM_all_c, (float)RAM_Hz / 1000000);
		
		///Thermal
		float PowerConsumption = ((batCurrentAvg / 1000) * (batVoltageAvg / 1000));
		if (GameRunning) {
			if (hosversionAtLeast(14,0,0)) {
				snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2d/%2d/%2.0f\u00B0C@%+.2fW", SOC_temperatureC, PCB_temperatureC, (float)skin_temperaturemiliC / 1000, PowerConsumption);
			}
			else
				snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.0f/%2.0f/%2.0f\u00B0C@%+.2fW", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000, PowerConsumption);
		}
		else {
			if (hosversionAtLeast(14,0,0)) {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2d/%2d/%2.1f\u00B0C@%+.2fW", SOC_temperatureC, PCB_temperatureC, (float)skin_temperaturemiliC / 1000, PowerConsumption);
			}
			else
				snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f/%2.1f/%2.1f\u00B0C@%+.2fW", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000, PowerConsumption);
		}
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.2f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%2.1f", FPSavg);

		//Debug
		/*
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "[100%s,100%s,100%s,100%s]@1785.0", "%", "%", "%", "%");
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "100.0%s@2400.0", "%");
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "4444/4444MB@4444.4");
		if (GameRunning) {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "88/88/88\u00B0C@15.55W");
		}
		else snprintf(skin_temperature_c, sizeof skin_temperature_c, "88.8/88.8/88.8\u00B0C@15.55W");
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "100.00%s", "%");
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "60.0");
		*/
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			TeslaFPS = 60;
			refreshrate = 60;
			CloseThreads();
			tsl::setNextOverlay(filepath);
			tsl::Overlay::get()->close();
			return true;
		}
		return false;
	}
};

//Battery
class BatteryOverlay : public tsl::Gui {
public:
    BatteryOverlay() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			if (R_SUCCEEDED(psmCheck)) {
				
				renderer->drawString("Battery/Charger Stats:", false, 20, 120, 20, renderer->a(0xFFFF));
				renderer->drawString(Battery_c, false, 20, 155, 18, renderer->a(0xFFFF));
			}

		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {

		///Battery

		if (_batteryChargeInfoFields.ChargerType)
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %+.0f mA\n"
				"Battery Average Power Flow: %+.3f W\n"
				"Charger Type: %u\n"
				"Charger Max Voltage: %u mV\n"
				"Charger Max Current: %u mA",
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
				batVoltageAvg,
				batCurrentAvg,
				((batVoltageAvg * batCurrentAvg) / 1'000'000),
				_batteryChargeInfoFields.ChargerType,
				_batteryChargeInfoFields.ChargerVoltageLimit,
				_batteryChargeInfoFields.ChargerCurrentLimit
			);
		else
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W",
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
				batVoltageAvg,
				batCurrentAvg,
				((batVoltageAvg * batCurrentAvg) / 1'000'000)
			);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			CloseThreads();
			svcSleepThread(500'000'000);
			tsl::goBack();
			return true;
		}
		return false;
	}
};

void StartMiscThread() {
	threadCreate(&t0, Misc2, NULL, NULL, 0x1000, 0x3F, 3);
	threadStart(&t0);
}

void EndMiscThread() {
	threadexit = true;
	threadWaitForExit(&t0);
	threadClose(&t0);
	threadexit = false;
}

class MiscOverlay : public tsl::Gui {
public:
    MiscOverlay() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			///DSP
			if (R_SUCCEEDED(audsnoopCheck)) {
				renderer->drawString(DSP_Load_c, false, 20, 120, 20, renderer->a(0xFFFF));
			}

			//NVDEC
			if (R_SUCCEEDED(nvdecCheck)) {
				renderer->drawString(NVDEC_Hz_c, false, 20, 165, 20, renderer->a(0xFFFF));
			}

			if (R_SUCCEEDED(nifmCheck)) {
				renderer->drawString("Network", false, 20, 210, 20, renderer->a(0xFFFF));
				if (!Nifm_internet_rc) {
					if (NifmConnectionType == NifmInternetConnectionType_WiFi) {
						renderer->drawString("Type: Wi-Fi", false, 20, 235, 18, renderer->a(0xFFFF));
						if (!Nifm_profile_rc) {
							if (Nifm_showpass)
								renderer->drawString(Nifm_pass, false, 20, 260, 15, renderer->a(0xFFFF));
							else
								renderer->drawString("Press Y to show password", false, 20, 260, 15, renderer->a(0xFFFF));
						}
					}
					else if (NifmConnectionType == NifmInternetConnectionType_Ethernet)
						renderer->drawString("Type: Ethernet", false, 20, 235, 18, renderer->a(0xFFFF));
				}
				else
					renderer->drawString("Type: Not connected", false, 20, 235, 18, renderer->a(0xFFFF));
		}

		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {

		snprintf(DSP_Load_c, sizeof DSP_Load_c, "DSP usage: %u%%", DSP_Load_u);
		snprintf(NVDEC_Hz_c, sizeof NVDEC_Hz_c, "NVDEC clock rate: %.1f MHz", (float)NVDEC_Hz / 1000);
		char pass_temp1[25] = "";
		char pass_temp2[25] = "";
		char pass_temp3[17] = "";
		if (Nifm_profile.wireless_setting_data.passphrase_len > 48) {
			memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
			memcpy(&pass_temp2, &(Nifm_profile.wireless_setting_data.passphrase[24]), 24);
			memcpy(&pass_temp3, &(Nifm_profile.wireless_setting_data.passphrase[48]), 16);
		}
		else if (Nifm_profile.wireless_setting_data.passphrase_len > 24) {
			memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
			memcpy(&pass_temp2, &(Nifm_profile.wireless_setting_data.passphrase[24]), 24);
		}
		else {
			memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
		}
		snprintf(Nifm_pass, sizeof Nifm_pass, "%s\n%s\n%s", pass_temp1, pass_temp2, pass_temp3);	
	}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_Y) {
			Nifm_showpass = true;
		}
		else Nifm_showpass = false;

		if (keysHeld & KEY_B) {
			EndMiscThread();
			nifmExit();
			svcSleepThread(500'000'000);
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Main Menu
class MainMenu : public tsl::Gui {
public:
    MainMenu() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);
		auto list = new tsl::elm::List();
		
		auto Full = new tsl::elm::ListItem("Full");
		Full->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				StartThreads();
				TeslaFPS = 1;
				refreshrate = 1;
				tsl::hlp::requestForeground(false);
				tsl::changeTo<FullOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Full);
		auto Mini = new tsl::elm::ListItem("Mini");
		Mini->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				StartThreads();
				TeslaFPS = 1;
				refreshrate = 1;
				alphabackground = 0x0;
				tsl::hlp::requestForeground(false);
				FullMode = false;
				tsl::changeTo<MiniOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Mini);
		FILE* test = fopen(filepath.c_str(), "rb");
		if (test) {
			fclose(test);
			auto Micro = new tsl::elm::ListItem("Micro");
			Micro->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::setNextOverlay(filepath, "--microOverlay");
					tsl::Overlay::get()->close();
					return true;
				}
				return false;
			});
			list->addItem(Micro);
		}
		if (SaltySD) {
			auto comFPS = new tsl::elm::ListItem("FPS Counter");
			comFPS->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					StartFPSCounterThread();
					TeslaFPS = 31;
					refreshrate = 31;
					alphabackground = 0x0;
					tsl::hlp::requestForeground(false);
					FullMode = false;
					tsl::changeTo<com_FPS>();
					return true;
				}
				return false;
			});
			list->addItem(comFPS);
		}
		auto Battery = new tsl::elm::ListItem("Battery/Charger");
		Battery->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				StartBatteryThread();
				tsl::changeTo<BatteryOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Battery);

		auto Misc = new tsl::elm::ListItem("Miscellaneous");
		Misc->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				smInitialize();
				nifmCheck = nifmInitialize(NifmServiceType_Admin);
				smExit();
				StartMiscThread();
				tsl::changeTo<MiscOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Misc);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {
		if (TeslaFPS != 60) {
			FullMode = true;
			tsl::hlp::requestForeground(true);
			TeslaFPS = 60;
			alphabackground = 0xD;
			refreshrate = 1;
			systemtickfrequency = 19200000;
		}
	}
    virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
    }
};

class MonitorOverlay : public tsl::Overlay {
public:

	virtual void initServices() override {
		//Initialize services
		tsl::hlp::doWithSmSession([this]{
			
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();
			
			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();

			if (R_SUCCEEDED(fanInitialize())) {
				if (hosversionAtLeast(7,0,0)) fanCheck = fanOpenController(&g_ICon, 0x3D000001);
				else fanCheck = fanOpenController(&g_ICon, 1);
			}

			if (R_SUCCEEDED(nvInitialize())) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");
			if (R_SUCCEEDED(nvMapInit())) nvdecCheck = nvChannelCreate(&nvdecChannel, "/dev/nvhost-nvdec");
			
			if (R_SUCCEEDED(audsnoopInitialize())) audsnoopCheck = audsnoopEnableDspUsageMeasurement();

			psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
				i2cInitialize();
			}
			
			SaltySD = CheckPort();
			
			if (SaltySD) {
				LoadSharedMemory();
				//Assign NX-FPS to default core
				threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
				
				//Start NX-FPS detection
				threadStart(&t6);
			}
		});
		Hinted = envIsSyscallHinted(0x6F);
	}

	virtual void exitServices() override {
		CloseThreads();
		shmemClose(&_sharedmemory);
		//Exit services
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanControllerClose(&g_ICon);
		fanExit();
		nvChannelClose(&nvdecChannel);
		nvMapExit();
		nvClose(fd);
		nvExit();
		i2cExit();
		psmExit();
		if (R_SUCCEEDED(audsnoopCheck)) {
			audsnoopDisableDspUsageMeasurement();
		}
		audsnoopExit();
	}

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MainMenu>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

class MicroMode : public tsl::Overlay {
public:

	virtual void initServices() override {
		//Initialize services
		tsl::hlp::doWithSmSession([this]{
			
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();
			
			if (R_SUCCEEDED(nvInitialize())) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");

			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();

			if (R_SUCCEEDED(fanInitialize())) {
				if (hosversionAtLeast(7,0,0)) fanCheck = fanOpenController(&g_ICon, 0x3D000001);
				else fanCheck = fanOpenController(&g_ICon, 1);
			}

			psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
				i2cInitialize();
			}
			
			SaltySD = CheckPort();
			
			if (SaltySD) {
				LoadSharedMemory();
				//Assign NX-FPS to default core
				threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
				
				//Start NX-FPS detection
				threadStart(&t6);
			}
		});
		Hinted = envIsSyscallHinted(0x6F);
	}

	virtual void exitServices() override {
		CloseThreads();
		shmemClose(&_sharedmemory);
		//Exit services
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanControllerClose(&g_ICon);
		fanExit();
		i2cExit();
		psmExit();
		nvClose(fd);
		nvExit();
	}

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
		StartThreads();
		refreshrate = 1;
        return initially<MicroOverlay>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

// This function gets called on startup to create a new Overlay object
int main(int argc, char **argv) {
	for (u8 arg = 0; arg < argc; arg++) {
		if (strcasecmp(argv[arg], "--microOverlay") == 0) {
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FullMode = false;
			alphabackground = 0x0;
			return tsl::loop<MicroMode>(argc, argv);
		}
	}
    return tsl::loop<MonitorOverlay>(argc, argv);
}
