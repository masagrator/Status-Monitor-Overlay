#include <tesla.hpp>
#include "services/tc.hpp"
#include "services/fan.hpp"
#include "libstratosphere/dmntcht.hpp"

//Common
Thread t0;
Thread t1;
Thread t2;
Thread t3;
Thread t4;
Thread t5;
Thread t6;
u64 systemtickfrequency = 19200000;
bool threadexit = false;
u64 refreshrate = 1;

//Checks
Result smCheck = 1;
Result clkrstCheck = 1;
Result nvCheck = 1;
Result pcvCheck = 1;
Result tsCheck = 1;
Result fanCheck = 1;
Result tcCheck = 1;
Result Hinted = 1;
Result pmdmntCheck = 1;
Result dmntchtCheck = 1;

//Temperatures
s32 SoC_temperaturemiliC = 0;
float SoC_temperatureC = 0;
s32 PCB_temperaturemiliC = 0;
float PCB_temperatureC = 0;
s32 skin_temperaturemiliC = 0;
float skin_temperatureC = 0;
char SoC_temperature_c[32];
char PCB_temperature_c[32];
char skin_temperature_c[32];

//CPU Usage
double percent = 0;
u64 idletick_a0 = 0;
u64 idletick_a1 = 0;
u64 idletick_a2 = 0;
u64 idletick_a3 = 0;
u64 idletick_b0 = 0;
u64 idletick_b1 = 0;
u64 idletick_b2 = 0;
u64 idletick_b3 = 0;
u64 idletick0 = 19200000;
u64 idletick1 = 19200000;
u64 idletick2 = 19200000;
u64 idletick3 = 19200000;
char CPU_Usage0[32];
char CPU_Usage1[32];
char CPU_Usage2[32];
char CPU_Usage3[32];

//Frequency
///CPU
u32 CPU_Hz = 0;
float CPU_Hz_f = 0;
char CPU_Hz_c[32];
///GPU
u32 GPU_Hz = 0;
float GPU_Hz_f = 0;
char GPU_Hz_c[32];
///RAM
u32 RAM_Hz = 0;
float RAM_Hz_f = 0;
char RAM_Hz_c[32];

//RAM Size
char RAM_all_c[64];
char RAM_application_c[64];
char RAM_applet_c[64];
char RAM_system_c[64];
char RAM_systemunsafe_c[64];
u64 RAM_Total_all_u = 0;
float RAM_Total_all_f = 0;
u64 RAM_Total_application_u = 0;
float RAM_Total_application_f = 0;
u64 RAM_Total_applet_u = 0;
float RAM_Total_applet_f = 0;
u64 RAM_Total_system_u = 0;
float RAM_Total_system_f = 0;
u64 RAM_Total_systemunsafe_u = 0;
float RAM_Total_systemunsafe_f = 0;
u64 RAM_Used_all_u = 0;
float RAM_Used_all_f = 0;
u64 RAM_Used_application_u = 0;
float RAM_Used_application_f = 0;
u64 RAM_Used_applet_u = 0;
float RAM_Used_applet_f = 0;
u64 RAM_Used_system_u = 0;
float RAM_Used_system_f = 0;
u64 RAM_Used_systemunsafe_u = 0;
float RAM_Used_systemunsafe_f = 0;

//Fan
float Rotation_SpeedLevel_f = 0;
float Rotation_SpeedLevel_percent = 0;
char Rotation_SpeedLevel_c[64];

//GPU Usage
u32 fd = 0;
u32 GPU_Load_u = 0;
float GPU_Load_percent = 0;
char GPU_Load_c[32];
float GPU_Load_max = 1000;

//FPS (WIP)
uintptr_t FPSaddress = 0x0;
uintptr_t FPSavgaddress = 0x0;
bool GameRunning = false;
char FPS_c[32];
uint8_t FPS = 0xFE;
char FPSavg_c[32];
float FPSavg = 255;
uint8_t check = 0;
bool SaltySD = false;

//Check if SaltyNX is working
bool CheckPort () {
	Result ret;
	Handle saltysd;
    for (int i = 0; i < 200; i++)
    {
        ret = svcConnectToNamedPort(&saltysd, "InjectServ");
        svcSleepThread(1000*1000);
        
        if (!ret) break;
    }
	svcCloseHandle(saltysd);
	if (ret != 0x0) return false;
	else return true;
}

void CheckIfGameRunning() {
	while (threadexit == false) {
		if (R_SUCCEEDED(pmdmntCheck) && R_SUCCEEDED(dmntchtCheck)) {
			Result rc = 1;
			uint64_t PID = 0;
			rc = pmdmntGetApplicationProcessId(&PID);
			if (R_FAILED(rc)) {
				if (check == 0) {
					remove("sdmc:/SaltySD/FPSoffset.hex");
					remove("sdmc:/SaltySD/FPSavgoffset.hex");
				}
				check = 1;
				GameRunning = false;
			}
			else if (GameRunning == false) {
				FILE* FPSoffset = fopen("sdmc:/SaltySD/FPSoffset.hex", "rb");
				FILE* FPSavgoffset = fopen("sdmc:/SaltySD/FPSavgoffset.hex", "rb");
				if ((FPSoffset != NULL) && (FPSavgoffset != NULL)) {
					dmntchtForceOpenCheatProcess();
					fread(&FPSaddress, 0x5, 1, FPSoffset);
					fread(&FPSavgaddress, 0x5, 1, FPSavgoffset);
					fclose(FPSoffset);
					fclose(FPSavgoffset);
					GameRunning = true;
					check = 0;
				}
			}
		}
		svcSleepThread(1000*1000*1000);
	}
}

//Check for input outside of FPS limitations
void CheckButtons() {
	while (threadexit == false) {
		hidScanInput();
		u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		if (kHeld & KEY_LSTICK) {
			hidScanInput();
			u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
			if (kHeld & KEY_RSTICK) tsl::Gui::goBack();
		}
		if (kHeld & KEY_ZR) {
			hidScanInput();
			u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
			if (kHeld & KEY_R) {
				hidScanInput();
				u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
				if (kHeld & KEY_DDOWN) {
					refreshrate = 1;
					tsl::Gui::divir(refreshrate);
					systemtickfrequency = 19200000;
				}
				else if (kHeld & KEY_DUP) {
					refreshrate = 5;
					tsl::Gui::divir(refreshrate);
					systemtickfrequency = 3840000;
				}
			}
		}
		svcSleepThread(100*1000*1000);
	}
}

//Stuff that doesn't need multithreading
void Misc() {
	while (threadexit == false) {
		
		// CPU, GPU and RAM Frequency
		if (R_SUCCEEDED(clkrstCheck)) {
			ClkrstSession cpuSession;
			clkrstOpenSession(&cpuSession, PcvModuleId_CpuBus, 3);
			clkrstGetClockRate(&cpuSession, &CPU_Hz);
			clkrstCloseSession(&cpuSession);
			clkrstOpenSession(&cpuSession, PcvModuleId_GPU, 3);
			clkrstGetClockRate(&cpuSession, &GPU_Hz);
			clkrstCloseSession(&cpuSession);
			clkrstOpenSession(&cpuSession, PcvModuleId_EMC, 3);
			clkrstGetClockRate(&cpuSession, &RAM_Hz);
			clkrstCloseSession(&cpuSession);
		}
		else if (R_SUCCEEDED(pcvCheck)) {
			pcvGetClockRate(PcvModule_CpuBus, &CPU_Hz);
			pcvGetClockRate(PcvModule_GPU, &GPU_Hz);
			pcvGetClockRate(PcvModule_EMC, &RAM_Hz);
		}
		
		//Temperatures
		if (R_SUCCEEDED(tsCheck)) {
			tsGetTemperatureMilliC(TsLocation_Internal, &SoC_temperaturemiliC);
			tsGetTemperatureMilliC(TsLocation_External, &PCB_temperaturemiliC);
		}
		if (R_SUCCEEDED(tcCheck)) tcGetTemperatureMilliC(&skin_temperaturemiliC);
		
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
		if (R_SUCCEEDED(fanCheck)) fanGetRotationSpeedLevel(&Rotation_SpeedLevel_f);
		
		//GPU Load
		if (R_SUCCEEDED(nvCheck)) nvIoctl(fd, 0x80044715, &GPU_Load_u);
		
		//FPS
		if (GameRunning == true) {
			dmntchtReadCheatProcessMemory(FPSaddress, &FPS, 0x1);
			dmntchtReadCheatProcessMemory(FPSavgaddress, &FPSavg, 0x4);
		}
		
		// 1 sec interval
		svcSleepThread(1000*1000*1000 / refreshrate);
	}
}

//Check each core for idled ticks in 1s intervals, they cannot read info about other core than they are assigned
void CheckCore0() {
	while (threadexit == false) {
		svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		svcSleepThread(1000*1000*1000 / refreshrate);
		svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
		idletick0 = idletick_a0 - idletick_b0;
	}
}

void CheckCore1() {
	while (threadexit == false) {
		svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		svcSleepThread(1000*1000*1000 / refreshrate);
		svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
		idletick1 = idletick_a1 - idletick_b1;
	}
}

void CheckCore2() {
	while (threadexit == false) {
		svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		svcSleepThread(1000*1000*1000 / refreshrate);
		svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
		idletick2 = idletick_a2 - idletick_b2;
	}
}

void CheckCore3() {
	while (threadexit == false) {
		svcGetInfo(&idletick_b3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
		svcSleepThread(1000*1000*1000 / refreshrate);
		svcGetInfo(&idletick_a3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
		idletick3 = idletick_a3 - idletick_b3;
	}
}

//Tesla stuff
class GuiMain : public tsl::Gui
{
public:
	GuiMain()
	{
		this->setTitle("Status Monitor");
	}
	~GuiMain() {}

	// Called when switching Guis to create the new UI
	virtual tsl::Element *createUI()
	{
		tsl::element::Frame *rootFrame = new tsl::element::Frame();

		tsl::element::CustomDrawer *Status = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
			
			//Print strings
			///CPU
			screen->drawString("CPU Usage:", false, 25, 100, 25, tsl::a(0xFFFF));
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) screen->drawString(CPU_Hz_c, false, 25, 135, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage0, false, 25, 165, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage1, false, 25, 180, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage2, false, 25, 195, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage3, false, 25, 210, 15, tsl::a(0xFFFF));
			
			///GPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(nvCheck)) {
				
				screen->drawString("GPU Usage:", false, 25, 265, 25, tsl::a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) screen->drawString(GPU_Hz_c, false, 25, 300, 15, tsl::a(0xFFFF));
				if (R_SUCCEEDED(nvCheck)) screen->drawString(GPU_Load_c, false, 25, 315, 15, tsl::a(0xFFFF));
				
			}
			
			///RAM
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(Hinted)) {
				
				screen->drawString("RAM Usage:", false, 25, 355, 25, tsl::a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) screen->drawString(RAM_Hz_c, false, 25, 390, 15, tsl::a(0xFFFF));
				if (R_SUCCEEDED(Hinted)) {
					screen->drawString(RAM_all_c, false, 25, 420, 15, tsl::a(0xFFFF));
					screen->drawString(RAM_application_c, false, 25, 435, 15, tsl::a(0xFFFF));
					screen->drawString(RAM_applet_c, false, 25, 450, 15, tsl::a(0xFFFF));
					screen->drawString(RAM_system_c, false, 25, 465, 15, tsl::a(0xFFFF));
					screen->drawString(RAM_systemunsafe_c, false, 25, 480, 15, tsl::a(0xFFFF));
				
				}
			}
			
			///Thermal
			if (R_SUCCEEDED(tsCheck) || R_SUCCEEDED(tcCheck) || R_SUCCEEDED(fanCheck)) {
				screen->drawString("Thermal:", false, 25, 520, 25, tsl::a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) {
					screen->drawString(SoC_temperature_c, false, 25, 555, 15, tsl::a(0xFFFF));
					screen->drawString(PCB_temperature_c, false, 25, 570, 15, tsl::a(0xFFFF));
				}
				if (R_SUCCEEDED(tcCheck)) screen->drawString(skin_temperature_c, false, 25, 585, 15, tsl::a(0xFFFF));
				if (R_SUCCEEDED(fanCheck)) screen->drawString(Rotation_SpeedLevel_c, false, 25, 600, 15, tsl::a(0xFFFF));
			}
			
			///FPS
			if (GameRunning == true) {
				screen->drawString(FPS_c, false, 235, 100, 20, tsl::a(0xFFFF));
				screen->drawString(FPSavg_c, false, 235, 120, 20, tsl::a(0xFFFF));
			}
			
			if (refreshrate == 5) screen->drawString("Hold ZR + R + D-Pad Down to slow down refresh", false, 20, 690, 15, tsl::a(0xFFFF));
			if (refreshrate == 1) screen->drawString("Hold ZR + R + D-Pad Up to speed up refresh", false, 20, 690, 15, tsl::a(0xFFFF));
		
	});

	rootFrame->addElement(Status);

	return rootFrame;
	}
	virtual void update() {
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		CPU_Hz_f = (float)CPU_Hz / (float)1000000;
		snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz", CPU_Hz_f);
		percent = (double) (((double)systemtickfrequency - (double)idletick0) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "Core #0: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick1) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "Core #1: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick2) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "Core #2: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick3) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "Core #3: %.2f%s", percent, "%");
		
		///GPU
		GPU_Load_percent = (float)GPU_Load_u / GPU_Load_max * 100;
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "Load: %.1f%s", GPU_Load_percent, "%");
		GPU_Hz_f = (float)GPU_Hz / (float)1000000;
		snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz", GPU_Hz_f);
		
		///RAM
		RAM_Hz_f = (float)RAM_Hz / (float)1000000;
		snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz", RAM_Hz_f);
		RAM_Total_application_f = (float)RAM_Total_application_u / 1024 / 1024;
		RAM_Total_applet_f = (float)RAM_Total_applet_u / 1024 / 1024;
		RAM_Total_system_f = (float)RAM_Total_system_u / 1024 / 1024;
		RAM_Total_systemunsafe_f = (float)RAM_Total_systemunsafe_u / 1024 / 1024;
		RAM_Total_all_f = RAM_Total_application_f + RAM_Total_applet_f + RAM_Total_system_f + RAM_Total_systemunsafe_f;
		RAM_Used_application_f = (float)RAM_Used_application_u / 1024 / 1024;
		RAM_Used_applet_f = (float)RAM_Used_applet_u / 1024 / 1024;
		RAM_Used_system_f = (float)RAM_Used_system_u / 1024 / 1024;
		RAM_Used_systemunsafe_f = (float)RAM_Used_systemunsafe_u / 1024 / 1024;
		RAM_Used_all_f = RAM_Used_application_f + RAM_Used_applet_f + RAM_Used_system_f + RAM_Used_systemunsafe_f;
		snprintf(RAM_all_c, sizeof RAM_all_c, "Total:               %.2f / %.2f MB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(RAM_application_c, sizeof RAM_application_c, "Application:      %.2f / %.2f MB", RAM_Used_application_f, RAM_Total_application_f);
		snprintf(RAM_applet_c, sizeof RAM_applet_c, "Applet:             %.2f / %.2f MB", RAM_Used_applet_f, RAM_Total_applet_f);
		snprintf(RAM_system_c, sizeof RAM_system_c, "System:            %.2f / %.2f MB", RAM_Used_system_f, RAM_Total_system_f);
		snprintf(RAM_systemunsafe_c, sizeof RAM_systemunsafe_c, "System Unsafe: %.2f / %.2f MB", RAM_Used_systemunsafe_f, RAM_Total_systemunsafe_f);
		
		///Thermal
		SoC_temperatureC = (float)SoC_temperaturemiliC / 1000;
		snprintf(SoC_temperature_c, sizeof SoC_temperature_c, "SoC: %.2f \u00B0C", SoC_temperatureC);
		PCB_temperatureC = (float)PCB_temperaturemiliC / 1000;
		snprintf(PCB_temperature_c, sizeof PCB_temperature_c, "PCB: %.2f \u00B0C", PCB_temperatureC);
		skin_temperatureC = (float)skin_temperaturemiliC / 1000;
		snprintf(skin_temperature_c, sizeof skin_temperature_c, "Skin: %.2f \u00B0C", skin_temperatureC);
		Rotation_SpeedLevel_percent = Rotation_SpeedLevel_f * 100;
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "Fan: %.2f%s", Rotation_SpeedLevel_percent, "%");
		
		///FPS
		snprintf(FPS_c, sizeof FPS_c, "FPR: %u", FPS);
		snprintf(FPSavg_c, sizeof FPSavg_c, "FPSavg: %.2f", FPSavg);
		
	}
};

class MonitorOverlay : public tsl::Overlay
{
public:
	MonitorOverlay() {}
	~MonitorOverlay() {}

	tsl::Gui *onSetup()
	{
		//Initialize services
		SaltySD = CheckPort();
		smCheck = smInitialize();
		if (R_SUCCEEDED(smCheck)) {
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();
			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();
			fanCheck = fanInitialize();
			nvCheck = nvInitialize();
			if (R_SUCCEEDED(nvCheck)) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");
			if (SaltySD == true) {
				FILE* disableflag = fopen("sdmc:/SaltySD/flags/disable.flag", "r");
				if (disableflag == NULL) {
					pmdmntCheck = pmdmntInitialize();
					dmntchtCheck = dmntchtInitialize();
				}
				else fclose(disableflag);
			}
		}
		Hinted = envIsSyscallHinted(0x6F);
		
		//Assign functions to core of choose
		threadCreate(&t0, CheckCore0, NULL, NULL, 0x100, 0x3B, 0);
		threadCreate(&t1, CheckCore1, NULL, NULL, 0x100, 0x3B, 1);
		threadCreate(&t2, CheckCore2, NULL, NULL, 0x100, 0x3B, 2);
		threadCreate(&t3, CheckCore3, NULL, NULL, 0x100, 0x3F, 3);
		threadCreate(&t4, Misc, NULL, NULL, 0x100, 0x3A, -2);
		threadCreate(&t5, CheckButtons, NULL, NULL, 0x200, 0x39, -2);
		threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
		
		//Start assigned functions
		threadStart(&t0);
		threadStart(&t1);
		threadStart(&t2);
		threadStart(&t3);
		threadStart(&t4);
		threadStart(&t5);
		threadStart(&t6);
		
		//Go to creating GUI
		return new GuiMain();
	}

	virtual void onDestroy() {
		//Give signal to exit for all threaded functions
		threadexit = true;
		
		//Wait for those functions to exit
		threadWaitForExit(&t0);
		threadWaitForExit(&t1);
		threadWaitForExit(&t2);
		threadWaitForExit(&t3);
		threadWaitForExit(&t4);
		threadWaitForExit(&t5);
		threadWaitForExit(&t6);
		
		//Exit services
		dmntchtExit();
		pmdmntExit();
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanExit();
		nvClose(fd);
		nvExit();
		smExit();

		
		//Free threads
		threadClose(&t0);
		threadClose(&t1);
		threadClose(&t2);
		threadClose(&t3);
		threadClose(&t4);
		threadClose(&t5);
		threadClose(&t6);
	}

	virtual void onOverlayShow(tsl::Gui *gui) {}

	void onOverlayHide(tsl::Gui *gui) {}
};

// This function gets called on startup to create a new Overlay object
tsl::Overlay *overlayLoad()
{
	return new MonitorOverlay();
}
