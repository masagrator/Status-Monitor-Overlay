#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "Utils.hpp"

ButtonMapperImpl buttonMapper; // Custom button mapper implementation
static bool returningFromSelection = false; // for removing the necessity of svcSleepThread
static tsl::elm::OverlayFrame* rootFrame = nullptr;
static bool skipMain = false;


//FPS Counter mode
class com_FPS : public tsl::Gui {
private:
	std::list<HidNpadButton> mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
	char FPSavg_c[8];
	FpsCounterSettings settings;
	size_t fontsize = 0;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
public:
    com_FPS() { 
		GetConfigSettings(&settings);
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::getRenderer().setLayerPos(624, 0);
				break;
			case 2:
			case 5:
			case 8:
				tsl::gfx::Renderer::getRenderer().setLayerPos(1248, 0);
				break;
		}
		StartFPSCounterThread();
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		refreshrate = TeslaFPS = settings.refreshRate;
	}
	~com_FPS() {
		refreshrate = TeslaFPS = 60;
		EndFPSCounterThread();
		if (settings.setPos)
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		systemtickfrequency = 19200000;
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			auto dimensions = renderer->drawString(FPSavg_c, false, 0, fontsize, fontsize, renderer->a(0x0000));
			size_t rectangleWidth = dimensions.first;
			size_t margin = (fontsize / 8);
			int base_x = 0;
			int base_y = 0;
			switch(settings.setPos) {
				case 1:
					base_x = 224 - ((rectangleWidth + margin) / 2);
					break;
				case 4:
					base_x = 224 - ((rectangleWidth + margin) / 2);
					base_y = 360 - ((fontsize + (margin / 2)) / 2);
					break;
				case 7:
					base_x = 224 - ((rectangleWidth + margin) / 2);
					base_y = 720 - (fontsize + (margin / 2));
					break;
				case 2:
					base_x = 448 - (rectangleWidth + margin);
					break;
				case 5:
					base_x = 448 - (rectangleWidth + margin);
					base_y = 360 - ((fontsize + (margin / 2)) / 2);
					break;
				case 8:
					base_x = 448 - (rectangleWidth + margin);
					base_y = 720 - (fontsize + (margin / 2));
					break;
			}
			renderer->drawRect(base_x, base_y, rectangleWidth + margin, fontsize + (margin / 2), a(settings.backgroundColor));
			renderer->drawString(FPSavg_c, false, base_x + (margin / 2), base_y+(fontsize-margin), fontsize, renderer->a(settings.textColor));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f", FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {

		bool allButtonsHeld = true;
		for (const HidNpadButton& button : mappedButtons) {
			if (!(keysHeld & static_cast<uint64_t>(button))) {
				allButtonsHeld = false;
				break;
			}
		}

		if (allButtonsHeld) {
			returningFromSelection = true;
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//FPS Graph mode
class com_FPSGraph : public tsl::Gui {
private:
	std::list<HidNpadButton> mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
	char FPSavg_c[8];
	FpsGraphSettings settings;
public:
    com_FPSGraph() { 
		GetConfigSettings(&settings);
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::getRenderer().setLayerPos(624, 0);
				break;
			case 2:
			case 5:
			case 8:
				tsl::gfx::Renderer::getRenderer().setLayerPos(1248, 0);
				break;
		}
		StartFPSCounterThread();
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		refreshrate = TeslaFPS = settings.refreshRate;
	}

	~com_FPSGraph() {
		EndFPSCounterThread();
		if (settings.setPos)
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		systemtickfrequency = 19200000;
	}

	struct stats {
		s16 value;
		bool zero_rounded;
	};

	std::vector<stats> readings;

	s16 base_y = 0;
	s16 base_x = 0;
	s16 rectangle_width = 180;
	s16 rectangle_height = 60;
	s16 rectangle_x = 15;
	s16 rectangle_y = 5;
	s16 rectangle_range_max = 60;
	s16 rectangle_range_min = 0;
	char legend_max[3] = "60";
	char legend_min[2] = "0";
	s32 range = std::abs(rectangle_range_max - rectangle_range_min) + 1;
	s16 x_end = rectangle_x + rectangle_width;
	s16 y_old = rectangle_y+rectangle_height;
	s16 y_30FPS = rectangle_y+(rectangle_height / 2);
	s16 y_60FPS = rectangle_y;
	bool isAbove = false;

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			switch(settings.setPos) {
				case 1:
					base_x = 224 - ((rectangle_width + 21) / 2);
					break;
				case 4:
					base_x = 224 - ((rectangle_width + 21) / 2);
					base_y = 360 - ((rectangle_height + 12) / 2);
					break;
				case 7:
					base_x = 224 - ((rectangle_width + 21) / 2);
					base_y = 720 - (rectangle_height + 12);
					break;
				case 2:
					base_x = 448 - (rectangle_width + 21);
					break;
				case 5:
					base_x = 448 - (rectangle_width + 21);
					base_y = 360 - ((rectangle_height + 12) / 2);
					break;
				case 8:
					base_x = 448 - (rectangle_width + 21);
					base_y = 720 - (rectangle_height + 12);
					break;
			}

			renderer->drawRect(base_x, base_y, rectangle_width + 21, rectangle_height + 12, a(settings.backgroundColor));
			if (FPSavg < 10) {
				renderer->drawString(FPSavg_c, false, base_x + 55, base_y+60, 63, renderer->a(settings.fpsColor));
			}
			else if (FPSavg < 100) {
				renderer->drawString(FPSavg_c, false, base_x + 35, base_y+60, 63, renderer->a(settings.fpsColor));
			} 
			else 
				renderer->drawString(FPSavg_c, false, base_x + 15, base_y+60, 63, renderer->a(settings.fpsColor));
			renderer->drawEmptyRect(base_x+(rectangle_x - 1), base_y+(rectangle_y - 1), rectangle_width + 2, rectangle_height + 4, renderer->a(settings.borderColor));
			renderer->drawDashedLine(base_x+rectangle_x, base_y+y_30FPS, base_x+rectangle_x+rectangle_width, base_y+y_30FPS, 6, renderer->a(settings.dashedLineColor));
			renderer->drawString(&legend_max[0], false, base_x+(rectangle_x-15), base_y+(rectangle_y+7), 10, renderer->a(settings.maxFPSTextColor));
			renderer->drawString(&legend_min[0], false, base_x+(rectangle_x-10), base_y+(rectangle_y+rectangle_height+3), 10, renderer->a(settings.minFPSTextColor));

			size_t last_element = readings.size() - 1;

			for (s16 x = x_end; x > static_cast<s16>(x_end-readings.size()); x--) {
				s32 y_on_range = readings[last_element].value + std::abs(rectangle_range_min) + 1;
				if (y_on_range < 0) {
					y_on_range = 0;
				}
				else if (y_on_range > range) {
					isAbove = true;
					y_on_range = range; 
				}
				
				s16 y = rectangle_y + static_cast<s16>(std::lround((float)rectangle_height * ((float)(range - y_on_range) / (float)range))); // 320 + (80 * ((61 - 61)/61)) = 320
				auto colour = renderer->a(settings.mainLineColor);
				if (y == y_old && !isAbove && readings[last_element].zero_rounded) {
					if ((y == y_30FPS || y == y_60FPS))
						colour = renderer->a(settings.perfectLineColor);
					else
						colour = renderer->a(settings.dashedLineColor);
				}

				if (x == x_end) {
					y_old = y;
				}
				/*
				else if (y - y_old > 0) {
					if (y_old + 1 <= rectangle_y+rectangle_height) 
						y_old += 1;
				}
				else if (y - y_old < 0) {
					if (y_old - 1 >= rectangle_y) 
						y_old -= 1;
				}
				*/

				renderer->drawLine(base_x+x, base_y+y, base_x+x, base_y+y_old, colour);
				isAbove = false;
				y_old = y;
				last_element--;
			}

		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		///FPS
		static float FPSavg_old = 0;
		stats temp = {0, false};

		if (FPSavg_old == FPSavg)
			return;
		FPSavg_old = FPSavg;
		snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f",  FPSavg);
		if (FPSavg < 254) {
			if ((s16)(readings.size()) >= rectangle_width) {
				readings.erase(readings.begin());
			}
			float whole = std::round(FPSavg);
			temp.value = static_cast<s16>(std::lround(FPSavg));
			if (FPSavg < whole+0.04 && FPSavg > whole-0.05) {
				temp.zero_rounded = true;
			}
			readings.push_back(temp);
		}
		else {
			readings.clear();
			readings.shrink_to_fit();
		}
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {

		bool allButtonsHeld = true;
		for (const HidNpadButton& button : mappedButtons) {
			if (!(keysHeld & static_cast<uint64_t>(button))) {
				allButtonsHeld = false;
				break;
			}
		}

		if (allButtonsHeld) {
			returningFromSelection = true;
			refreshrate = TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Full mode
class FullOverlay : public tsl::Gui {
private:
	std::list<HidNpadButton> mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
	char RealCPU_Hz_c[32];
	char DeltaCPU_c[12];
	char DeltaGPU_c[12];
	char DeltaRAM_c[12];
	char RealGPU_Hz_c[32];
	char RealRAM_Hz_c[32];
	char GPU_Load_c[32];
	char Rotation_SpeedLevel_c[64];
	char RAM_compressed_c[64];
	char RAM_var_compressed_c[128];
	char CPU_Hz_c[64];
	char GPU_Hz_c[64];
	char RAM_Hz_c[64];
	char CPU_compressed_c[160];
	char CPU_Usage0[32];
	char CPU_Usage1[32];
	char CPU_Usage2[32];
	char CPU_Usage3[32];
	char SoCPCB_temperature_c[64];
	char skin_temperature_c[32];
	char BatteryDraw_c[64];
	char FPS_var_compressed_c[64];
	char RAM_load_c[64];

	uint8_t COMMON_MARGIN = 20;
public:
    FullOverlay() { 
		StartThreads();
		tsl::hlp::requestForeground(false);
		refreshrate = TeslaFPS = 1;
	}
	~FullOverlay() {
		CloseThreads();
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		systemtickfrequency = 19200000;
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			//Print strings
			///CPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {

				uint32_t height_offset = 155;
				if (realCPU_Hz) {
					height_offset = 162;
				}
				renderer->drawString("CPU Usage:", false, COMMON_MARGIN, 120, 20, renderer->a(0xFFFF));
				auto dimensions = renderer->drawString("Target Frequency: ", false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
				uint32_t offset = COMMON_MARGIN + dimensions.first;
				renderer->drawString(CPU_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));
				if (realCPU_Hz) {
					renderer->drawString("Real Frequency:", false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
					renderer->drawString(RealCPU_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
					renderer->drawString(DeltaCPU_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
				}
				renderer->drawString(CPU_compressed_c, false, COMMON_MARGIN, height_offset + 30, 15, renderer->a(0xFFFF));
			}
			
			///GPU
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(nvCheck)) {
				
				uint32_t height_offset = 320;
				if (realGPU_Hz) {
					height_offset = 327;
				}

				renderer->drawString("GPU Usage:", false, COMMON_MARGIN, 285, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {
					auto dimensions = renderer->drawString("Target Frequency: ", false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
					uint32_t offset = COMMON_MARGIN + dimensions.first;
					renderer->drawString(GPU_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));
					if (realCPU_Hz) {
						renderer->drawString("Real Frequency: ", false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(RealGPU_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(DeltaGPU_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
					}
				}
				if (R_SUCCEEDED(nvCheck)) {
					renderer->drawString(GPU_Load_c, false, COMMON_MARGIN, height_offset + 15, 15, renderer->a(0xFFFF));
				}
				
			}
			
			///RAM
			if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck) || R_SUCCEEDED(Hinted)) {
				
				uint32_t height_offset = 410;
				if (realRAM_Hz) {
					height_offset += 7;
				}

				renderer->drawString("RAM Usage:", false, COMMON_MARGIN, 375, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(clkrstCheck) || R_SUCCEEDED(pcvCheck)) {
					auto dimensions = renderer->drawString("Target Frequency: ", false, COMMON_MARGIN, height_offset, 15, renderer->a(0xFFFF));
					uint32_t offset = COMMON_MARGIN + dimensions.first;
					renderer->drawString(RAM_Hz_c, false, offset, height_offset, 15, renderer->a(0xFFFF));
					if (realRAM_Hz) {
						renderer->drawString("Real Frequency:", false, COMMON_MARGIN, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(RealRAM_Hz_c, false, offset, height_offset - 15, 15, renderer->a(0xFFFF));
						renderer->drawString(DeltaRAM_c, false, COMMON_MARGIN + 230, height_offset - 7, 15, renderer->a(0xFFFF));
					}
					if (sysClkApiVer >= 4) {
						renderer->drawString(RAM_load_c, false, COMMON_MARGIN, height_offset+15, 15, renderer->a(0xFFFF));
					}
				}
				if (R_SUCCEEDED(Hinted)) {
					renderer->drawString(RAM_compressed_c, false, COMMON_MARGIN, height_offset + 30, 15, renderer->a(0xFFFF));
					renderer->drawString(RAM_var_compressed_c, false, COMMON_MARGIN + 120, height_offset + 30, 15, renderer->a(0xFFFF));
				}
			}
			
			///Thermal
			if (R_SUCCEEDED(tsCheck) || R_SUCCEEDED(tcCheck) || R_SUCCEEDED(fanCheck)) {
				renderer->drawString("Board:", false, 20, 540, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString(BatteryDraw_c, false, COMMON_MARGIN, 575, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString("Temperatures: SoC\n\t\t\t\t\t\t\t PCB\n\t\t\t\t\t\t\t Skin", false, COMMON_MARGIN, 590, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(tsCheck)) renderer->drawString(SoCPCB_temperature_c, false, COMMON_MARGIN + 150, 590, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(fanCheck)) renderer->drawString(Rotation_SpeedLevel_c, false, COMMON_MARGIN, 635, 15, renderer->a(0xFFFF));
			}
			
			///FPS
			if (GameRunning) {
				uint32_t width_offset = 230;
				auto dimensions = renderer->drawString("PFPS: \nFPS:", false, COMMON_MARGIN + width_offset, 120, 20, renderer->a(0xFFFF));
				uint32_t offset = COMMON_MARGIN + width_offset + dimensions.first;
				renderer->drawString(FPS_var_compressed_c, false, offset, 120, 20, renderer->a(0xFFFF));
			}
			
			std::string formattedKeyCombo = keyCombo;
			formatButtonCombination(formattedKeyCombo);
			
			std::string message = "Hold " + formattedKeyCombo + " to Exit";
			
			renderer->drawString(message.c_str(), false, COMMON_MARGIN, 675, 15, renderer->a(0xFFFF));
			
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		snprintf(CPU_Hz_c, sizeof(CPU_Hz_c), "%d.%d MHz", CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
		if (realCPU_Hz) {
			snprintf(RealCPU_Hz_c, sizeof(RealCPU_Hz_c), "%d.%d MHz", realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
			int32_t deltaCPU = realCPU_Hz - CPU_Hz;
			snprintf(DeltaCPU_c, sizeof(DeltaCPU_c), "Δ %d.%d", deltaCPU / 1000000, abs(deltaCPU / 100000) % 10);
		}
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "Core #0: %.2f%%", (1.d - ((double)idletick0 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "Core #1: %.2f%%", (1.d - ((double)idletick1 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "Core #2: %.2f%%", (1.d - ((double)idletick2 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "Core #3: %.2f%%", (1.d - ((double)idletick3 / systemtickfrequency)) * 100);
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "%s\n%s\n%s\n%s", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3);
		
		///GPU
		snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "%d.%d MHz", GPU_Hz / 1000000, (GPU_Hz / 100000) % 10);
		if (realGPU_Hz) {
			snprintf(RealGPU_Hz_c, sizeof(RealGPU_Hz_c), "%d.%d MHz", realGPU_Hz / 1000000, (realGPU_Hz / 100000) % 10);
			int32_t deltaGPU = realGPU_Hz - GPU_Hz;
			snprintf(DeltaGPU_c, sizeof(DeltaGPU_c), "Δ %u.%u", deltaGPU / 1000000, abs(deltaGPU / 100000) % 10);
		}
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "Load: %d.%d%%", GPU_Load_u / 10, GPU_Load_u % 10);
		
		///RAM
		snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "%d.%d MHz", RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
		if (realRAM_Hz) {
			snprintf(RealRAM_Hz_c, sizeof(RealRAM_Hz_c), "%d.%d MHz", realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
			int32_t deltaRAM = realRAM_Hz - RAM_Hz;
			snprintf(DeltaRAM_c, sizeof(DeltaRAM_c), "Δ %d.%d", deltaRAM / 1000000, abs(deltaRAM / 100000) % 10);
		}
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
		snprintf(RAM_compressed_c, sizeof RAM_compressed_c, "Total:\nApplication:\nApplet:\nSystem:\nSystem Unsafe:");
		char FULL_RAM_all_c[21] = "";
		char FULL_RAM_application_c[21] = "";
		char FULL_RAM_applet_c[21] = "";
		char FULL_RAM_system_c[21] = "";
		char FULL_RAM_systemunsafe_c[21] = "";
		snprintf(FULL_RAM_all_c, sizeof(FULL_RAM_all_c), "%4.2f / %4.2f MB", RAM_Used_all_f, RAM_Total_all_f);
		snprintf(FULL_RAM_application_c, sizeof(FULL_RAM_application_c), "%4.2f / %4.2f MB", RAM_Used_application_f, RAM_Total_application_f);
		snprintf(FULL_RAM_applet_c, sizeof(FULL_RAM_applet_c), "%4.2f / %4.2f MB", RAM_Used_applet_f, RAM_Total_applet_f);
		snprintf(FULL_RAM_system_c, sizeof(FULL_RAM_system_c), "%4.2f / %4.2f MB", RAM_Used_system_f, RAM_Total_system_f);
		snprintf(FULL_RAM_systemunsafe_c, sizeof(FULL_RAM_systemunsafe_c), "%4.2f / %4.2f MB", RAM_Used_systemunsafe_f, RAM_Total_systemunsafe_f);
		snprintf(RAM_var_compressed_c, sizeof(RAM_var_compressed_c), "%s\n%s\n%s\n%s\n%s", FULL_RAM_all_c, FULL_RAM_application_c, FULL_RAM_applet_c, FULL_RAM_system_c, FULL_RAM_systemunsafe_c);
		
		if (sysClkApiVer >= 4) {
			int RAM_GPU_Load = _sysclkemcload.load[SysClkEmcLoad_All] - _sysclkemcload.load[SysClkEmcLoad_Cpu];
			snprintf(RAM_load_c, sizeof RAM_load_c, 
				"Load: %d.%d%% (CPU %d.%d | GPU %d.%d)", 
				_sysclkemcload.load[SysClkEmcLoad_All] / 10, _sysclkemcload.load[SysClkEmcLoad_All] % 10,
				_sysclkemcload.load[SysClkEmcLoad_Cpu] / 10, _sysclkemcload.load[SysClkEmcLoad_Cpu] % 10,
				RAM_GPU_Load / 10, RAM_GPU_Load % 10);
		}
		///Thermal
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		snprintf(BatteryDraw_c, sizeof BatteryDraw_c, "Battery Power Flow: %+.2fW[%s]", PowerConsumption, remainingBatteryLife);
		if (hosversionAtLeast(10,0,0)) {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, 
				"%2.1f \u00B0C\n%2.1f \u00B0C\n%2d.%d \u00B0C", 
				SOC_temperatureF, PCB_temperatureF, skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		else {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, 
				"%2d.%d \u00B0C\n%2d.%d\u00B0C\n%2d.%d \u00B0C", 
				SOC_temperatureC / 1000, (SOC_temperatureC / 100) % 10, 
				PCB_temperatureC / 1000, (PCB_temperatureC % 100) % 10,
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "Fan Rotation Level:\t%2.1f%%", Rotation_SpeedLevel_f * 100);
		
		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%u\n%2.1f", FPS, FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {

		bool allButtonsHeld = true;
		for (const HidNpadButton& button : mappedButtons) {
			if (!(keysHeld & static_cast<uint64_t>(button))) {
				allButtonsHeld = false;
				break;
			}
		}

		if (allButtonsHeld) {
			returningFromSelection = true;
			refreshrate = TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

//Mini mode
class MiniOverlay : public tsl::Gui {
private:
	std::list<HidNpadButton> mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
	char GPU_Load_c[32];
	char Rotation_SpeedLevel_c[64];
	char RAM_var_compressed_c[128];
	char SoCPCB_temperature_c[64];
	char skin_temperature_c[32];

	uint32_t rectangleWidth = 0;
	char Variables[512];
	size_t fontsize = 0;
	MiniSettings settings;
	bool Initialized = false;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
public:
    MiniOverlay() { 
		GetConfigSettings(&settings);
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::getRenderer().setLayerPos(624, 0);
				break;
			case 2:
			case 5:
			case 8:
				tsl::gfx::Renderer::getRenderer().setLayerPos(1248, 0);
				break;
		}
		StartThreads();
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		refreshrate = TeslaFPS = settings.refreshRate;
	}
	~MiniOverlay() {
		CloseThreads();
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		systemtickfrequency = 19200000;
	}

    virtual tsl::elm::Element* createUI() override {

		rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			if (!Initialized) {
				std::pair<u32, u32> dimensions;
				for (std::string key : tsl::hlp::split(settings.show, '+')) {
					if (!key.compare("CPU")) {
						dimensions = renderer->drawString("[100%,100%,100%,100%]@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("GPU") || (!key.compare("RAM") && settings.showRAMLoad)) {
						dimensions = renderer->drawString("100.0%@4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("RAM") && !settings.showRAMLoad) {
						dimensions = renderer->drawString("4444/4444MB@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("TEMP")) {
						dimensions = renderer->drawString("88.8\u00B0C/88.8\u00B0C/88.8\u00B0C", false, 0, fontsize, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("DRAW")) {
						dimensions = renderer->drawString("-44.44W[44:44]", false, 0, fontsize, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("FAN")) {
						dimensions = renderer->drawString("100.0%", false, 0, fontsize, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
					else if (!key.compare("FPS")) {
						dimensions = renderer->drawString("444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
						if (rectangleWidth < dimensions.first)
							rectangleWidth = dimensions.first;
					}
				}
				Initialized = true;
			}
			
			char print_text[24] = "";
			size_t entry_count = 0;
			uint8_t flags = 0;
			for (std::string key : tsl::hlp::split(settings.show, '+')) {
				if (!key.compare("CPU") && !(flags & 1 << 0)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "CPU");
					entry_count++;
					flags |= (1 << 0);
				}
				else if (!key.compare("GPU") && !(flags & 1 << 1)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "GPU");
					entry_count++;
					flags |= (1 << 1);
				}
				else if (!key.compare("RAM") && !(flags & 1 << 2)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "RAM");
					entry_count++;
					flags |= (1 << 2);
				}
				else if (!key.compare("TEMP") && !(flags & 1 << 3)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "TEMP");
					entry_count++;
					flags |= (1 << 3);
				}
				else if (!key.compare("DRAW") && !(flags & 1 << 4)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "DRAW");
					entry_count++;
					flags |= (1 << 4);
				}
				else if (!key.compare("FAN") && !(flags & 1 << 5)) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "FAN");
					entry_count++;
					flags |= (1 << 5);
				}
				else if (!key.compare("FPS") && !(flags & 1 << 6) && GameRunning) {
					if (print_text[0])
						strcat(print_text, "\n");
					strcat(print_text, "FPS");
					entry_count++;
					flags |= (1 << 6);
				}
			}

			uint32_t height = (fontsize * entry_count) + (fontsize / 3);
			uint32_t margin = (fontsize * 4);

			int base_x = 0;
			int base_y = 0;
			switch(settings.setPos) {
				case 1:
					base_x = 224 - ((margin + rectangleWidth + (fontsize / 3)) / 2);
					break;
				case 4:
					base_x = 224 - ((margin + rectangleWidth + (fontsize / 3)) / 2);
					base_y = 360 - height / 2;
					break;
				case 7:
					base_x = 224 - ((margin + rectangleWidth + (fontsize / 3)) / 2);
					base_y = 720 - height;
					break;
				case 2:
					base_x = 448 - (margin + rectangleWidth + (fontsize / 3));
					break;
				case 5:
					base_x = 448 - (margin + rectangleWidth + (fontsize / 3));
					base_y = 360 - height / 2;
					break;
				case 8:
					base_x = 448 - (margin + rectangleWidth + (fontsize / 3));
					base_y = 720 - height;
					break;
			}
			
			renderer->drawRect(base_x, base_y, margin + rectangleWidth + (fontsize / 3), height, a(settings.backgroundColor));
			renderer->drawString(print_text, false, base_x, base_y + fontsize, fontsize, renderer->a(settings.catColor));
			
			///GPU
			renderer->drawString(Variables, false, base_x + margin, base_y + fontsize, fontsize, renderer->a(settings.textColor));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		char MINI_CPU_Usage0[7] = "";
		char MINI_CPU_Usage1[7] = "";
		char MINI_CPU_Usage2[7] = "";
		char MINI_CPU_Usage3[7] = "";

		snprintf(MINI_CPU_Usage0, sizeof(MINI_CPU_Usage0), "%.0f%%", (1.d - ((double)idletick0 / systemtickfrequency)) * 100);
		snprintf(MINI_CPU_Usage1, sizeof(MINI_CPU_Usage1), "%.0f%%", (1.d - ((double)idletick1 / systemtickfrequency)) * 100);
		snprintf(MINI_CPU_Usage2, sizeof(MINI_CPU_Usage2), "%.0f%%", (1.d - ((double)idletick2 / systemtickfrequency)) * 100);
		snprintf(MINI_CPU_Usage3, sizeof(MINI_CPU_Usage3), "%.0f%%", (1.d - ((double)idletick3 / systemtickfrequency)) * 100);
		
		char MINI_CPU_compressed_c[42] = "";
		if (settings.realFrequencies && realCPU_Hz) {
			snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c), 
				"[%s,%s,%s,%s]@%hu.%hhu", 
				MINI_CPU_Usage0, MINI_CPU_Usage1, MINI_CPU_Usage2, MINI_CPU_Usage3, 
				realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
		}
		else {
			snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c), 
				"[%s,%s,%s,%s]@%hu.%hhu", 
				MINI_CPU_Usage0, MINI_CPU_Usage1, MINI_CPU_Usage2, MINI_CPU_Usage3, 
				CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
		}
		char MINI_GPU_Load_c[14];
		if (settings.realFrequencies && realGPU_Hz) {
			snprintf(MINI_GPU_Load_c, sizeof(MINI_GPU_Load_c), 
				"%hu.%hhu%%@%hu.%hhu", 
				GPU_Load_u / 10, GPU_Load_u % 10,
				realGPU_Hz / 1000000, (realGPU_Hz / 100000) % 10);
		}
		else {
			snprintf(MINI_GPU_Load_c, sizeof(MINI_GPU_Load_c), 
				"%hu.%hhu%%@%hu.%hhu", 
				GPU_Load_u / 10, GPU_Load_u % 10, 
				GPU_Hz / 1000000, (GPU_Hz / 100000) % 10);
		}
		
		///RAM
		char MINI_RAM_var_compressed_c[19] = "";
		if (sysClkApiVer < 4 || !settings.showRAMLoad) {
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
			if (settings.realFrequencies && realRAM_Hz) {
				snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), 
					"%.0f/%.0fMB@%hu.%hhu", 
					RAM_Used_all_f, RAM_Total_all_f, 
					realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
			}
			else {
				snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), 
					"%.0f/%.0fMB@%hu.%hhu",
					RAM_Used_all_f, RAM_Total_all_f, 
					RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
			}
		}
		else {
			if (settings.realFrequencies && realRAM_Hz) {
				snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), 
					"%hu.%hhu%%@%hu.%hhu", 
					_sysclkemcload.load[SysClkEmcLoad_All] / 10, _sysclkemcload.load[SysClkEmcLoad_All] % 10, 
					realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
			}
			else {
				snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), 
					"%hu.%hhu%%@%hu.%hhu", 
					_sysclkemcload.load[SysClkEmcLoad_All] / 10, _sysclkemcload.load[SysClkEmcLoad_All] % 10, 
					RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
			}
		}
		
		///Thermal
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		
		snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%0.2fW[%s]", PowerConsumption, remainingBatteryLife);
		if (hosversionAtLeast(10,0,0)) {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, 
				"%2.1f\u00B0C/%2.1f\u00B0C/%hu.%hhu\u00B0C", 
				SOC_temperatureF, PCB_temperatureF, 
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		else {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, 
				"%hu.%hhu\u00B0C/%hu.%hhu\u00B0C/%hu.%hhu\u00B0C", 
				SOC_temperatureC / 1000, (SOC_temperatureC / 100) % 10, 
				PCB_temperatureC / 1000, (PCB_temperatureC / 100) % 10, 
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		}
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%%", Rotation_SpeedLevel_f * 100);
		
		///FPS
		char Temp[256] = "";
		uint8_t flags = 0;
		for (std::string key : tsl::hlp::split(settings.show, '+')) {
			if (!key.compare("CPU") && !(flags & 1 << 0)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, MINI_CPU_compressed_c);
				flags |= 1 << 0;			
			}
			else if (!key.compare("GPU") && !(flags & 1 << 1)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, MINI_GPU_Load_c);
				flags |= 1 << 1;			
			}
			else if (!key.compare("RAM") && !(flags & 1 << 2)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, MINI_RAM_var_compressed_c);
				flags |= 1 << 2;			
			}
			else if (!key.compare("TEMP") && !(flags & 1 << 3)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, skin_temperature_c);
				flags |= 1 << 3;			
			}
			else if (!key.compare("FAN") && !(flags & 1 << 4)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, Rotation_SpeedLevel_c);
				flags |= 1 << 4;			
			}
			else if (!key.compare("DRAW") && !(flags & 1 << 5)) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				strcat(Temp, SoCPCB_temperature_c);
				flags |= 1 << 5;			
			}
			else if (!key.compare("FPS") && !(flags & 1 << 6) && GameRunning) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				char Temp_s[8] = "";
				snprintf(Temp_s, sizeof(Temp_s), "%2.1f", FPSavg);
				strcat(Temp, Temp_s);
				flags |= 1 << 6;			
			}
		}
		strcpy(Variables, Temp);

	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		
		bool allButtonsHeld = true;
		for (const HidNpadButton& button : mappedButtons) {
			if (!(keysHeld & static_cast<uint64_t>(button))) {
				allButtonsHeld = false;
				break;
			}
		}

		if (allButtonsHeld) {
			returningFromSelection = true;
			refreshrate = TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

//Micro mode
class MicroOverlay : public tsl::Gui {
private:
	std::list<HidNpadButton> mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
	char GPU_Load_c[32];
	char Rotation_SpeedLevel_c[64];
	char RAM_var_compressed_c[128];
	char CPU_compressed_c[160];
	char CPU_Usage0[32];
	char CPU_Usage1[32];
	char CPU_Usage2[32];
	char CPU_Usage3[32];
	char skin_temperature_c[32];
	char batteryCharge[10]; // Declare the batteryCharge variable
	char FPS_var_compressed_c[64];

	uint32_t margin = 8;

	std::pair<u32, u32> CPU_dimensions;
	std::pair<u32, u32> GPU_dimensions;
	std::pair<u32, u32> RAM_dimensions;
	std::pair<u32, u32> BRD_dimensions;
	std::pair<u32, u32> FAN_dimensions;
	std::pair<u32, u32> FPS_dimensions;
	bool Initialized = false;
	MicroSettings settings;
	size_t text_width = 0;
	size_t fps_width = 0;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
	size_t fontsize = 0;
	bool showFPS = false;
public:
    MicroOverlay() { 
		GetConfigSettings(&settings);
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		if (settings.setPosBottom) {
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 1038);
		}
		StartThreads();
		tsl::hlp::requestForeground(false);
		refreshrate = TeslaFPS = settings.refreshRate;
		alphabackground = 0x0;
	}
	~MicroOverlay() {
		CloseThreads();
	}
    
    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {

			if (!Initialized) {
				CPU_dimensions = renderer->drawString("CPU [100%,100%,100%,100%]△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				GPU_dimensions = renderer->drawString("GPU 100.0%△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				if (!settings.showRAMLoad) {
					RAM_dimensions = renderer->drawString("RAM 44.4/44.4GB△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				}
				else RAM_dimensions = renderer->drawString("RAM 100.0%△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				BRD_dimensions = renderer->drawString("BRD 88.8/88.8/88.8\u00B0C@-15.5W[99:99]", false, 0, fontsize, fontsize, renderer->a(0x0000));
				FAN_dimensions = renderer->drawString("FAN 100.0%", false, 0, fontsize, fontsize, renderer->a(0x0000));
				FPS_dimensions = renderer->drawString("FPS 44.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				auto spacesize = renderer->drawString(" ", false, 0, fontsize, fontsize, renderer->a(0x0000));
				margin = spacesize.first;
				int8_t entry_count = -1;
				uint8_t flags = 0;
				for (std::string key : tsl::hlp::split(settings.show, '+')) {
					if (!key.compare("CPU") && !(flags & 1 << 0)) {
						text_width += CPU_dimensions.first;
						entry_count += 1;
						flags |= 1 << 0;
					}
					else if (!key.compare("GPU") && !(flags & 1 << 1)) {
						text_width += GPU_dimensions.first;
						entry_count += 1;
						flags |= 1 << 1;
					}
					else if (!key.compare("RAM") && !(flags & 1 << 2)) {
						text_width += RAM_dimensions.first;
						entry_count += 1;
						flags |= 1 << 2;
					}
					else if (!key.compare("BRD") && !(flags & 1 << 3)) {
						text_width += BRD_dimensions.first;
						entry_count += 1;
						flags |= 1 << 3;
					}
					else if (!key.compare("FAN") && !(flags & 1 << 4)) {
						text_width += FAN_dimensions.first;
						entry_count += 1;
						flags |= 1 << 4;
					}
					else if (!key.compare("FPS") && !(flags & 1 << 5)) {
						fps_width = FPS_dimensions.first;
						showFPS = true;
						flags |= 1 << 5;
					}
				}
				text_width += (margin * entry_count);
				Initialized = true;
			}

			u32 base_y = 0;
			if (settings.setPosBottom) {
				base_y = tsl::cfg::FramebufferHeight - (fontsize + (fontsize / 4));
			}

			renderer->drawRect(0, base_y, tsl::cfg::FramebufferWidth, fontsize + (fontsize / 4), a(settings.backgroundColor));

			uint32_t offset = 0;
			if (settings.alignTo == 1) {
				if (GameRunning && showFPS) {
					offset = (tsl::cfg::FramebufferWidth - (text_width + fps_width)) / 2;
				}
				else offset = (tsl::cfg::FramebufferWidth - text_width) / 2;
			}
			else if (settings.alignTo == 2) {
				if (GameRunning && showFPS) {
					offset = tsl::cfg::FramebufferWidth - (text_width + fps_width);
				}
				else offset = tsl::cfg::FramebufferWidth - text_width;
			}
			uint8_t flags = 0;
			for (std::string key : tsl::hlp::split(settings.show, '+')) {
				if (!key.compare("CPU") && !(flags & 1 << 0)) {
					auto dimensions_s = renderer->drawString("CPU", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(CPU_compressed_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += CPU_dimensions.first + margin;
					flags |= 1 << 0;
				}
				else if (!key.compare("GPU") && !(flags & 1 << 1)) {
					auto dimensions_s = renderer->drawString("GPU", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(GPU_Load_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += GPU_dimensions.first + margin;
					flags |= 1 << 1;
				}
				else if (!key.compare("RAM") && !(flags & 1 << 2)) {
					auto dimensions_s = renderer->drawString("RAM", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(RAM_var_compressed_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += RAM_dimensions.first + margin;
					flags |= 1 << 2;
				}
				else if (!key.compare("BRD") && !(flags & 1 << 3)) {
					auto dimensions_s = renderer->drawString("BRD", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(skin_temperature_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += BRD_dimensions.first + margin;
					flags |= 1 << 3;
				}
				else if (!key.compare("FAN") && !(flags & 1 << 4)) {
					auto dimensions_s = renderer->drawString("FAN", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(Rotation_SpeedLevel_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += FAN_dimensions.first + margin;
					flags |= 1 << 4;
				}
				else if (!key.compare("FPS") && GameRunning && !(flags & 1 << 5)) {
					auto dimensions_s = renderer->drawString("FPS", false, offset, base_y+fontsize, fontsize, renderer->a(settings.catColor));
					uint32_t offset_s = offset + dimensions_s.first + margin;
					renderer->drawString(FPS_var_compressed_c, false, offset_s, base_y+fontsize, fontsize, renderer->a(settings.textColor));
					offset += FPS_dimensions.first + margin;
					flags |= 1 << 5;
				}
			}
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			fontsize = settings.dockedFontSize;
		}
		//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
		//This is because making each loop also takes time, which is not considered because this will take also additional time
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		
		//Make stuff ready to print
		///CPU
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "%.0f%%", (1.d - ((double)idletick0 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "%.0f%%", (1.d - ((double)idletick1 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "%.0f%%", (1.d - ((double)idletick2 / systemtickfrequency)) * 100);
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "%.0f%%", (1.d - ((double)idletick3 / systemtickfrequency)) * 100);

		char difference[5] = "@";
		if (realCPU_Hz) {
			int32_t deltaCPU = realCPU_Hz - CPU_Hz;
			if (deltaCPU > 20000000) {
				strcpy(difference, "△");
			}
			else if (deltaCPU < -50000000) {
				strcpy(difference, "≠");
			}
			else if (deltaCPU < -20000000) {
				strcpy(difference, "▽");
			}
		}
		if (settings.realFrequencies && realCPU_Hz) {
			snprintf(CPU_compressed_c, sizeof CPU_compressed_c, 
				"[%s,%s,%s,%s]%s%d.%d", 
				CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3, 
				difference, 
				realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
		}
		else {
			snprintf(CPU_compressed_c, sizeof CPU_compressed_c, 
				"[%s,%s,%s,%s]%s%d.%d", 
				CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3, 
				difference, 
				CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
		}
		
		///GPU
		if (realGPU_Hz) {
			int32_t deltaGPU = realGPU_Hz - GPU_Hz;
			if (deltaGPU >= 20000000) {
				strcpy(difference, "△");
			}
			else if (deltaGPU > -20000000) {
				strcpy(difference, "@");
			}
			else if (deltaGPU < -50000000) {
				strcpy(difference, "≠");
			}
			else if (deltaGPU < -20000000) {
				strcpy(difference, "▽");
			}
		}
		else {
			strcpy(difference, "@");
		}
		if (settings.realFrequencies && realGPU_Hz) {
			snprintf(GPU_Load_c, sizeof GPU_Load_c, 
				"%d.%d%%%s%d.%d", 
				GPU_Load_u / 10, GPU_Load_u % 10, 
				difference, 
				realGPU_Hz / 1000000, (realGPU_Hz / 100000) % 10);
		}
		else {
			snprintf(GPU_Load_c, sizeof GPU_Load_c, 
				"%d.%d%%%s%d.%d", 
				GPU_Load_u / 10, GPU_Load_u % 10, 
				difference, 
				GPU_Hz / 1000000, (GPU_Hz / 100000) % 10);
		}
		
		///RAM
		char MICRO_RAM_all_c[12] = "";
		if (!settings.showRAMLoad || sysClkApiVer < 4) {
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
			snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%.1f/%.1fGB", RAM_Used_all_f/1024, RAM_Total_all_f/1024);
		}
		else {
			snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%hu.%hhu%%", _sysclkemcload.load[SysClkEmcLoad_All] / 10, _sysclkemcload.load[SysClkEmcLoad_All] % 10);
		}

		if (realRAM_Hz) {
			int32_t deltaRAM = realRAM_Hz - RAM_Hz;
			if (deltaRAM >= 20000000) {
				strcpy(difference, "△");
			}
			else if (deltaRAM > -20000000) {
				strcpy(difference, "@");
			}
			else if (deltaRAM < -50000000) {
				strcpy(difference, "≠");
			}
			else if (deltaRAM < -20000000) {
				strcpy(difference, "▽");
			}
		}
		else {
			strcpy(difference, "@");
		}
		if (settings.realFrequencies && realRAM_Hz) {
			snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, 
				"%s%s%d.%d", 
				MICRO_RAM_all_c, difference, realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
		}
		else {
			 snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, 
				"%s%s%d.%d", 
				MICRO_RAM_all_c, difference, RAM_Hz / 1000000, (RAM_Hz / 1000000) % 10);
		}
		
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");

		///Thermal
		if (hosversionAtLeast(10,0,0)) {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, 
				"%2.1f/%2.1f/%hu.%hhu\u00B0C@%+.1fW[%s]", 
				SOC_temperatureF, PCB_temperatureF, 
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10, 
				PowerConsumption, remainingBatteryLife);
		}
		else {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, 
				"%hu.%hhu/%hu.%hhu/%hu.%hhu\u00B0C@%+.1fW[%s]", 
				SOC_temperatureC / 1000, (SOC_temperatureC / 100) % 10, 
				PCB_temperatureC / 1000, (PCB_temperatureC / 100) % 10, 
				skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10, 
				PowerConsumption, remainingBatteryLife);
		}
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%%", Rotation_SpeedLevel_f * 100);
		
		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%2.1f", FPSavg);
		
		
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		//std::list<HidNpadButton> mappedButtons;
		//ButtonMapperImpl buttonMapper; // Create an instance of the ButtonMapperImpl class
		//mappedButtons = buttonMapper.MapButtons(keyCombo); // map buttons
		
		bool allButtonsHeld = true;
		for (const HidNpadButton& button : mappedButtons) {
			if (!(keysHeld & static_cast<uint64_t>(button))) {
				allButtonsHeld = false;
				break;
			}
		}

		if (allButtonsHeld) {
			returningFromSelection = true;
			refreshrate = TeslaFPS = 60;
            if (skipMain)
                tsl::goBack();
            else {
			    tsl::setNextOverlay(filepath.c_str());
			    tsl::Overlay::get()->close();
            }
			return true;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

//Battery
class BatteryOverlay : public tsl::Gui {
private:
	char Battery_c[512];
public:
    BatteryOverlay() {
		StartBatteryThread();
	}
	~BatteryOverlay() {
		CloseThreads();
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
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

		char tempBatTimeEstimate[8] = "-:--";
		if (batTimeEstimate >= 0) {
			snprintf(&tempBatTimeEstimate[0], sizeof(tempBatTimeEstimate), "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}

		BatteryChargeInfoFieldsChargerType ChargerConnected = _batteryChargeInfoFields.ChargerType;
		int32_t ChargerVoltageLimit = _batteryChargeInfoFields.ChargerVoltageLimit;
		int32_t ChargerCurrentLimit = _batteryChargeInfoFields.ChargerCurrentLimit;
		if (hosversionAtLeast(17,0,0)) {
			ChargerConnected = ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields) -> ChargerType;
			ChargerVoltageLimit = ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields) -> ChargerVoltageLimit;
			ChargerCurrentLimit = ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields) -> ChargerCurrentLimit;
		}
		if (ChargerConnected)
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Actual Capacity: %.0f mAh\n"
				"Battery Designed Capacity: %.0f mAh\n"
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%%\n"
				"Battery Age: %.1f%%\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %+.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W\n"
				"Battery Remaining Time: %s\n"
				"Charger Type: %u\n"
				"Charger Max Voltage: %u mV\n"
				"Charger Max Current: %u mA",
				actualFullBatCapacity,
				designedFullBatCapacity,
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000,
				(float)_batteryChargeInfoFields.BatteryAge / 1000,
				batVoltageAvg,
				batCurrentAvg,
				PowerConsumption, 
				tempBatTimeEstimate,
				ChargerConnected,
				ChargerVoltageLimit,
				ChargerCurrentLimit
			);
		else
			snprintf(Battery_c, sizeof Battery_c,
				"Battery Actual Capacity: %.0f mAh\n"
				"Battery Designed Capacity: %.0f mAh\n"
				"Battery Temperature: %.1f\u00B0C\n"
				"Battery Raw Charge: %.1f%%\n"
				"Battery Age: %.1f%%\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W\n"
				"Battery Remaining Time: %s",
				actualFullBatCapacity,
				designedFullBatCapacity,
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000,
				(float)_batteryChargeInfoFields.BatteryAge / 1000,
				batVoltageAvg,
				batCurrentAvg,
				PowerConsumption, 
				tempBatTimeEstimate
			);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			returningFromSelection = true;
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
private:
	char DSP_Load_c[16];
	char NVDEC_Hz_c[18];
	char NVENC_Hz_c[18];
	char NVJPG_Hz_c[18];
	char Nifm_pass[96];
public:
    MiscOverlay() { 
		smInitialize();
		nifmCheck = nifmInitialize(NifmServiceType_Admin);
		if (R_SUCCEEDED(mmuInitialize())) {
			nvdecCheck = mmuRequestInitialize(&nvdecRequest, MmuModuleId(5), 8, false);
			nvencCheck = mmuRequestInitialize(&nvencRequest, MmuModuleId(6), 8, false);
			nvjpgCheck = mmuRequestInitialize(&nvjpgRequest, MmuModuleId(7), 8, false);
		}

		if (R_SUCCEEDED(audsnoopInitialize())) 
			audsnoopCheck = audsnoopEnableDspUsageMeasurement();

		smExit();
		StartMiscThread();
	}

	~MiscOverlay() {
		EndMiscThread();
		nifmExit();
		mmuRequestFinalize(&nvdecRequest);
		mmuRequestFinalize(&nvencRequest);
		mmuRequestFinalize(&nvjpgRequest);
		mmuExit();
		if (R_SUCCEEDED(audsnoopCheck)) {
			audsnoopDisableDspUsageMeasurement();
		}
		audsnoopExit();
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			///DSP
			if (R_SUCCEEDED(audsnoopCheck)) {
				renderer->drawString(DSP_Load_c, false, 20, 120, 20, renderer->a(0xFFFF));
			}

			//Multimedia engines
			if (R_SUCCEEDED(nvdecCheck | nvencCheck | nvjpgCheck)) {
				renderer->drawString("Multimedia clock rates:", false, 20, 165, 20, renderer->a(0xFFFF));
				if (R_SUCCEEDED(nvdecCheck))
					renderer->drawString(NVDEC_Hz_c, false, 35, 185, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(nvencCheck))
					renderer->drawString(NVENC_Hz_c, false, 35, 200, 15, renderer->a(0xFFFF));
				if (R_SUCCEEDED(nvjpgCheck))
					renderer->drawString(NVJPG_Hz_c, false, 35, 215, 15, renderer->a(0xFFFF));
			}

			if (R_SUCCEEDED(nifmCheck)) {
				renderer->drawString("Network", false, 20, 255, 20, renderer->a(0xFFFF));
				if (!Nifm_internet_rc) {
					if (NifmConnectionType == NifmInternetConnectionType_WiFi) {
						renderer->drawString("Type: Wi-Fi", false, 20, 280, 18, renderer->a(0xFFFF));
						if (!Nifm_profile_rc) {
							if (Nifm_showpass)
								renderer->drawString(Nifm_pass, false, 20, 305, 15, renderer->a(0xFFFF));
							else
								renderer->drawString("Press Y to show password", false, 20, 305, 15, renderer->a(0xFFFF));
						}
					}
					else if (NifmConnectionType == NifmInternetConnectionType_Ethernet)
						renderer->drawString("Type: Ethernet", false, 20, 280, 18, renderer->a(0xFFFF));
				}
				else
					renderer->drawString("Type: Not connected", false, 20, 280, 18, renderer->a(0xFFFF));
		}

		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {

		snprintf(DSP_Load_c, sizeof DSP_Load_c, "DSP usage: %u%%", DSP_Load_u);
		snprintf(NVDEC_Hz_c, sizeof NVDEC_Hz_c, "NVDEC: %.1f MHz", (float)NVDEC_Hz / 1000000);
		snprintf(NVENC_Hz_c, sizeof NVENC_Hz_c, "NVENC: %.1f MHz", (float)NVENC_Hz / 1000000);
		snprintf(NVJPG_Hz_c, sizeof NVJPG_Hz_c, "NVJPG: %.1f MHz", (float)NVJPG_Hz / 1000000);
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
			returningFromSelection = true;
			tsl::goBack();
			return true;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

//Graphs
class GraphsMenu : public tsl::Gui {
public:
    GraphsMenu() {}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "Graphs");
		auto list = new tsl::elm::List();

		auto comFPSGraph = new tsl::elm::ListItem("FPS");
		comFPSGraph->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<com_FPSGraph>();
				return true;
			}
			return false;
		});
		list->addItem(comFPSGraph);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			returningFromSelection = true;
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Other
class OtherMenu : public tsl::Gui {
public:
    OtherMenu() { }

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "Other");
		auto list = new tsl::elm::List();

		auto Battery = new tsl::elm::ListItem("Battery/Charger");
		Battery->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<BatteryOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Battery);

		auto Misc = new tsl::elm::ListItem("Miscellaneous");
		Misc->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<MiscOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Misc);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (!returningFromSelection && (keysHeld & KEY_B)) {
			returningFromSelection = true;
			tsl::goBack();
			return true;
		}
		if (returningFromSelection && !(keysHeld & KEY_B)) {
			returningFromSelection = false;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

//Main Menu
class MainMenu : public tsl::Gui {
public:
    MainMenu() {}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);
		auto list = new tsl::elm::List();
		
		auto Full = new tsl::elm::ListItem("Full");
		Full->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<FullOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Full);
		auto Mini = new tsl::elm::ListItem("Mini");
		Mini->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<MiniOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Mini);

		bool fileExist = false;
		FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
		if (test) {
			fclose(test);
			fileExist = true;
			filepath = folderpath + filename;
		}
		else {
			test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
			if (test) {
				fclose(test);
				fileExist = true;
				filepath = folderpath + "Status-Monitor-Overlay.ovl";
			}
		}
		if (fileExist) {
			auto Micro = new tsl::elm::ListItem("Micro");
			Micro->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::setNextOverlay(filepath, "--microOverlay_");
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
					tsl::changeTo<com_FPS>();
					return true;
				}
				return false;
			});
			list->addItem(comFPS);
			auto Graphs = new tsl::elm::ListItem("Graphs");
			Graphs->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::changeTo<GraphsMenu>();
					return true;
				}
				return false;
			});
			list->addItem(Graphs);
		}
		auto Other = new tsl::elm::ListItem("Other");
		Other->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<OtherMenu>();
				return true;
			}
			return false;
		});
		list->addItem(Other);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {
		if (tsl::cfg::LayerPosX || tsl::cfg::LayerPosY) {
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		}
	}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (!returningFromSelection && (keysHeld & KEY_B)) {
			tsl::goBack();
			return true;
		}
		if (returningFromSelection && !(keysHeld & KEY_B)) {
			returningFromSelection = false;
		}
		if (keysHeld & KEY_B) {
			return false;
		}
		return false;
	}
};

class MonitorOverlay : public tsl::Overlay {
public:

	virtual void initServices() override {
		//Initialize services
		tsl::hlp::doWithSmSession([this]{
			
			apmInitialize();
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();
			
			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();

			if (R_SUCCEEDED(fanInitialize())) {
				if (hosversionAtLeast(7,0,0)) fanCheck = fanOpenController(&g_ICon, 0x3D000001);
				else fanCheck = fanOpenController(&g_ICon, 1);
			}

			if (R_SUCCEEDED(nvInitialize())) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");

			psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
				i2cInitialize();
			}
			
			SaltySD = CheckPort();
			
			if (SaltySD) {
				LoadSharedMemory();
			}
			if (sysclkIpcRunning() && R_SUCCEEDED(sysclkIpcInitialize())) {
				sysclkIpcGetAPIVersion(&sysClkApiVer);
				if (sysClkApiVer < 3) {
					sysclkIpcExit();
				}
				else sysclkCheck = 0;
			}
		});
		Hinted = envIsSyscallHinted(0x6F);
	}

	virtual void exitServices() override {
		CloseThreads();
		if (R_SUCCEEDED(sysclkCheck)) {
			sysclkIpcExit();
		}
		shmemClose(&_sharedmemory);
		//Exit services
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanControllerClose(&g_ICon);
		fanExit();
		nvMapExit();
		nvClose(fd);
		nvExit();
		i2cExit();
		psmExit();
		apmExit();
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
			
			apmInitialize();
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
			}
			if (sysclkIpcRunning() && R_SUCCEEDED(sysclkIpcInitialize())) {
				sysclkIpcGetAPIVersion(&sysClkApiVer);
				if (sysClkApiVer < 3) {
					sysclkIpcExit();
				}
				else sysclkCheck = 0;
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
		apmExit();
	}

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MicroOverlay>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};



// This function gets called on startup to create a new Overlay object
int main(int argc, char **argv) {
	ParseIniFile(); // parse INI from file
    
	if (argc > 0) {
		filename = argv[0];
	}
	for (u8 arg = 0; arg < argc; arg++) {
		if (strcasecmp(argv[arg], "--microOverlay_") == 0) {
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
			if (test) {
				fclose(test);
				filepath = folderpath + filename;
			}
			else {
				test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
				if (test) {
					fclose(test);
					filepath = folderpath + "Status-Monitor-Overlay.ovl";
				}
			}
			return tsl::loop<MicroMode>(argc, argv);
		} else if (strcasecmp(argv[arg], "--microOverlay") == 0) {
            skipMain = true;
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
			if (test) {
				fclose(test);
				filepath = folderpath + filename;
			}
			else {
				test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
				if (test) {
					fclose(test);
					filepath = folderpath + "Status-Monitor-Overlay.ovl";
				}
			}
			return tsl::loop<MicroMode>(argc, argv);
		}
	}
    return tsl::loop<MonitorOverlay>(argc, argv);
}
