#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "Utils.hpp"

//FPS Counter mode
class com_FPS : public tsl::Gui {
public:
    com_FPS() { }

	s16 base_y = 0;

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
				static uint8_t avg = 0;
				if (FPSavg >= 100) avg = 46;
				else if (FPSavg >= 10) avg = 23;
				else avg = 0;
				renderer->drawRect(0, base_y, tsl::cfg::FramebufferWidth - 370 + avg, 50, a(0x7111));
				renderer->drawString(FPSavg_c, false, 5, base_y+40, 40, renderer->a(0xFFFF));
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
		else if ((keysHeld & KEY_ZR) && (keysHeld & KEY_R)) {
			if ((keysHeld & KEY_DUP) && base_y != 0) {
				base_y = 0;
			}
			else if ((keysHeld & KEY_DDOWN) && base_y != 670) {
				base_y = 670;
			}
		}
		return false;
	}
};

//FPS Counter mode
class com_FPSGraph : public tsl::Gui {
public:
    com_FPSGraph() { }

	struct stats {
		s16 value;
		bool zero_rounded;
	};

	std::vector<stats> readings;

	s16 base_y = 0;
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
		auto rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			renderer->drawRect(0, base_y, 201, 72, a(0x7111));
			if (FPSavg < 10) {
				renderer->drawString(FPSavg_c, false, 55, base_y+60, 63, renderer->a(0x4444));
			}
			else if (FPSavg < 100) {
				renderer->drawString(FPSavg_c, false, 35, base_y+60, 63, renderer->a(0x4444));
			} 
			else 
				renderer->drawString(FPSavg_c, false, 15, base_y+60, 63, renderer->a(0x4444));
			renderer->drawEmptyRect(rectangle_x - 1, base_y+rectangle_y - 1, rectangle_width + 2, rectangle_height + 4, renderer->a(0xF77F));
			renderer->drawDashedLine(rectangle_x, base_y+y_30FPS, rectangle_x+rectangle_width, base_y+y_30FPS, 6, renderer->a(0x8888));
			renderer->drawString(&legend_max[0], false, rectangle_x-15, base_y+rectangle_y+7, 10, renderer->a(0xFFFF));
			renderer->drawString(&legend_min[0], false, rectangle_x-10, base_y+rectangle_y+rectangle_height+3, 10, renderer->a(0xFFFF));

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
				auto colour = renderer->a(0xFFFF);
				if (y == y_old && !isAbove && readings[last_element].zero_rounded) {
					if ((y == y_30FPS || y == y_60FPS))
						colour = renderer->a(0xF0C0);
					else
						colour = renderer->a(0xFF0F);
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

				renderer->drawLine(x, base_y+y, x, base_y+y_old, colour);
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
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			EndFPSCounterThread();
			tsl::goBack();
			return true;
		}
		else if ((keysHeld & KEY_ZR) && (keysHeld & KEY_R)) {
			if ((keysHeld & KEY_DUP) && base_y != 0) {
				base_y = 0;
			}
			else if ((keysHeld & KEY_DDOWN) && base_y != 648) {
				base_y = 648;
			}
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
		if (realCPU_Hz == 0) {
			snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz (Δ 0.0)", (float)CPU_Hz / 1000000);
		}
		else {
			int32_t deltaCPU = realCPU_Hz - CPU_Hz;
			if ((deltaCPU < 100000 && deltaCPU > -100000)) {
				snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz (Δ 0.0)", (float)CPU_Hz / 1000000);
			}
			else
				snprintf(CPU_Hz_c, sizeof CPU_Hz_c, "Frequency: %.1f MHz (Δ%+.1f)", (float)CPU_Hz / 1000000, (float)deltaCPU / 1000000);
		}
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "Core #0: %.2f%s", ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "Core #1: %.2f%s", ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "Core #2: %.2f%s", ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "Core #3: %.2f%s", ((double)systemtickfrequency - (double)idletick3) / (double)systemtickfrequency * 100, "%");
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "%s\n%s\n%s\n%s", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3);
		
		///GPU
		if (realGPU_Hz == 0) {
			snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz", (float)GPU_Hz / 1000000);
		}
		else {
			int32_t deltaGPU = realGPU_Hz - GPU_Hz;
			if (deltaGPU < 100000 && deltaGPU > -100000) {
				snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz (Δ 0.0)", (float)GPU_Hz / 1000000);
			}
			else
				snprintf(GPU_Hz_c, sizeof GPU_Hz_c, "Frequency: %.1f MHz (Δ%+.1f)", (float)GPU_Hz / 1000000, (float)deltaGPU / 1000000);
		}
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "Load: %.1f%s", (float)GPU_Load_u / 10, "%");
		
		///RAM
		int32_t deltaRAM = realRAM_Hz - RAM_Hz;
		if (realRAM_Hz == 0) {
			snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz", (float)RAM_Hz / 1000000);
		}
		else {
			if (deltaRAM < 100000 && deltaRAM > -100000) {
				snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz (Δ 0.0)", (float)RAM_Hz / 1000000);
			}
			else
				snprintf(RAM_Hz_c, sizeof RAM_Hz_c, "Frequency: %.1f MHz (Δ%+.1f)", (float)RAM_Hz / 1000000, (float)deltaRAM / 1000000);
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
		
		///Thermal
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		snprintf(BatteryDraw_c, sizeof BatteryDraw_c, "Battery Power Flow: %+.2fW[%s]", PowerConsumption, remainingBatteryLife);
		if (hosversionAtLeast(10,0,0)) {
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%2.1f \u00B0C\n%2.1f \u00B0C\n%2.1f \u00B0C", SOC_temperatureF, PCB_temperatureF, (float)skin_temperaturemiliC / 1000);
		}
		else 
			snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%2.1f \u00B0C\n%2.1f\u00B0C\n%2.1f \u00B0C", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "Fan Rotation Level:\t%2.1f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_c, sizeof FPS_c, "PFPS:"); //Pushed Frames Per Second
		snprintf(FPSavg_c, sizeof FPSavg_c, "FPS:"); //Frames Per Second calculated from averaged frametime 
		snprintf(FPS_compressed_c, sizeof FPS_compressed_c, "%s\n%s", FPS_c, FPSavg_c);
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%u\n%2.1f", FPS, FPSavg);
		
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
			
			uint32_t height = 95;
			if (realCPU_Hz || realGPU_Hz || realRAM_Hz) {
				height += 15;
			}
			if (GameRunning) {
				height += 30;
			}
			renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth - 150, height, a(0x7111));
			
			//Print strings
			///CPU
			std::string print_text = "CPU\nGPU\nRAM\n";
			if (realCPU_Hz || realGPU_Hz || realRAM_Hz) {
				print_text += "DIFF\n";
			}
			print_text += "TEMP\nFAN\nDRAW";
			if (GameRunning) {
				print_text += "\nPFPS\nFPS";
			}
			else renderer->drawString(print_text.c_str(), false, 0, 15, 15, renderer->a(0xFFFF));
			
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
		char MINI_CPU_Usage0[7] = "";
		char MINI_CPU_Usage1[7] = "";
		char MINI_CPU_Usage2[7] = "";
		char MINI_CPU_Usage3[7] = "";

		double percent = ((double)systemtickfrequency - (double)idletick0) / (double)systemtickfrequency * 100;
		snprintf(MINI_CPU_Usage0, sizeof(MINI_CPU_Usage0), "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick1) / (double)systemtickfrequency * 100;
		snprintf(MINI_CPU_Usage1, sizeof(MINI_CPU_Usage1), "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick2) / (double)systemtickfrequency * 100;
		snprintf(MINI_CPU_Usage2, sizeof(MINI_CPU_Usage2), "%.0f%s", percent, "%");
		percent = ((double)systemtickfrequency - (double)idletick3) / (double)systemtickfrequency * 100;
		snprintf(MINI_CPU_Usage3, sizeof(MINI_CPU_Usage3), "%.0f%s", percent, "%");
		
		char MINI_CPU_compressed_c[42] = "";
		snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c), "[%s,%s,%s,%s]@%.1f", MINI_CPU_Usage0, MINI_CPU_Usage1, MINI_CPU_Usage2, MINI_CPU_Usage3, (float)CPU_Hz / 1000000);
		char MINI_GPU_Load_c[14];
		snprintf(MINI_GPU_Load_c, sizeof(MINI_GPU_Load_c), "%.1f%s@%.1f", (float)GPU_Load_u / 10, "%", (float)GPU_Hz / 1000000);
		
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
		char MINI_RAM_var_compressed_c[18] = "";
		snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), "%.0f/%.0fMB@%.1f", RAM_Used_all_f, RAM_Total_all_f, (float)RAM_Hz / 1000000);

		char DIFF_compressed_c[22] = "";
		if (realCPU_Hz || realGPU_Hz || realRAM_Hz) {
			int deltaCPU = realCPU_Hz - CPU_Hz;
			int deltaGPU = realGPU_Hz - GPU_Hz;
			int deltaRAM = realRAM_Hz - RAM_Hz;
			snprintf(DIFF_compressed_c, sizeof(DIFF_compressed_c), "%+2.1f %+2.1f %+2.1f", (float)deltaCPU / 1000000, (float)deltaGPU / 1000000, (float)deltaRAM / 1000000);
		}
		
		///Thermal
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		
		snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%0.2fW[%s]", PowerConsumption, remainingBatteryLife);
		if (hosversionAtLeast(10,0,0))
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f\u00B0C/%2.1f\u00B0C/%2.1f\u00B0C", SOC_temperatureF, PCB_temperatureF, (float)skin_temperaturemiliC / 1000);
		else
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f\u00B0C/%2.1f\u00B0C/%2.1f\u00B0C", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_c, sizeof FPS_c, "PFPS:"); //Pushed Frames Per Second
		snprintf(FPSavg_c, sizeof FPSavg_c, "FPS:"); //Frames Per Second calculated from averaged frametime 
		snprintf(FPS_compressed_c, sizeof FPS_compressed_c, "%s\n%s", FPS_c, FPSavg_c);
		snprintf(FPS_var_compressed_c, sizeof FPS_compressed_c, "%u\n%2.1f", FPS, FPSavg);
		char Temp[71] = "";
		snprintf(Temp, sizeof(Temp), "%s\n%s\n%s", MINI_CPU_compressed_c, MINI_GPU_Load_c, MINI_RAM_var_compressed_c);
		char Temp2[93] = "";
		if (realCPU_Hz || realGPU_Hz || realRAM_Hz) {
			snprintf(Temp2, sizeof(Temp2), "%s\n%s", Temp, DIFF_compressed_c);
		}
		else strcpy(Temp2, Temp);
		char Temp3[256] = "";
		snprintf(Temp3, sizeof(Temp3), "%s\n%s\n%s\n%s", Temp2, skin_temperature_c, Rotation_SpeedLevel_c, SoCPCB_temperature_c);
		if (GameRunning) {
			snprintf(Variables, sizeof(Variables), "%s\n%s", Temp3, FPS_var_compressed_c);
		}
		else strcpy(Variables, Temp3);

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
				uint32_t offset4 = offset3 + 245;
				uint32_t offset5 = offset4 + 360;
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
				uint32_t offset2 = offset1 + 343;
				uint32_t offset3 = offset2 + 197;
				uint32_t offset4 = offset3 + 218;
				uint32_t offset5 = offset4 + 328;
				uint32_t offset6 = offset5 + 116;
				renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth, 22, a(0x7111));
				renderer->drawString("CPU", false, offset1, size, size, renderer->a(0xFCCF));
				renderer->drawString("GPU", false, offset2, size, size, renderer->a(0xFCCF));
				renderer->drawString("RAM", false, offset3, size, size, renderer->a(0xFCCF));
				renderer->drawString("BRD", false, offset4, size, size, renderer->a(0xFCCF));
				renderer->drawString("FAN", false, offset5, size, size, renderer->a(0xFCCF));
				renderer->drawString("FPS", false, offset6, size, size, renderer->a(0xFCCF));
				renderer->drawString(CPU_compressed_c, false, offset1+42, size, size, renderer->a(0xFFFF));
				renderer->drawString(GPU_Load_c, false, offset2+42, size, size, renderer->a(0xFFFF));
				renderer->drawString(RAM_var_compressed_c, false, offset3+47, size, size, renderer->a(0xFFFF));
				renderer->drawString(skin_temperature_c, false, offset4+42, size, size, renderer->a(0xFFFF));
				renderer->drawString(Rotation_SpeedLevel_c, false, offset5+43, size, size, renderer->a(0xFFFF));
				renderer->drawString(FPS_var_compressed_c, false, offset6+38, size, size, renderer->a(0xFFFF));
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

		char difference[5] = "@";
		if (realCPU_Hz) {
			int32_t deltaCPU = realCPU_Hz - CPU_Hz;
			if (deltaCPU > 20000000) {
				strcpy(difference, "▲");
			}
			else if (deltaCPU > 100000) {
				strcpy(difference, "△");
			}
			else if (deltaCPU < -100000) {
				strcpy(difference, "▽");
			}
			else if (deltaCPU < -20000000) {
				strcpy(difference, "▼");
			}
			else if (deltaCPU < -50000000) {
				strcpy(difference, "◘");
			}
		}
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "[%s,%s,%s,%s]%s%.1f", CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3, difference, (float)CPU_Hz / 1000000);
		
		///GPU
		if (realGPU_Hz) {
			int32_t deltaGPU = realGPU_Hz - GPU_Hz;
			if (deltaGPU >= 20000000) {
				strcpy(difference, "▲");
			}
			else if (deltaGPU >= 100000) {
				strcpy(difference, "△");
			}
			else if (deltaGPU > -100000) {
				strcpy(difference, "@");
			}
			else if (deltaGPU <= -100000) {
				strcpy(difference, "▽");
			}
			else if (deltaGPU <= -20000000) {
				strcpy(difference, "▼");
			}
			else if (deltaGPU <= -50000000) {
				strcpy(difference, "◘");
			}
		}
		else {
			strcpy(difference, "@");
		}
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "%.1f%s%s%.1f", (float)GPU_Load_u / 10, "%", difference, (float)GPU_Hz / 1000000);
		
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
		char MICRO_RAM_all_c[12] = "";
		snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%.1f/%.1fGB", RAM_Used_all_f/1024, RAM_Total_all_f/1024);

		if (realRAM_Hz) {
			int32_t deltaRAM = realRAM_Hz - RAM_Hz;
			if (deltaRAM >= 20000000) {
				strcpy(difference, "▲");
			}
			else if (deltaRAM >= 100000) {
				strcpy(difference, "△");
			}
			else if (deltaRAM > -100000) {
				strcpy(difference, "@");
			}
			else if (deltaRAM <= -100000) {
				strcpy(difference, "▽");
			}
			else if (deltaRAM <= -20000000) {
				strcpy(difference, "▼");
			}
			else if (deltaRAM <= -50000000) {
				strcpy(difference, "◘");
			}
		}
		else {
			strcpy(difference, "@");
		}
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "%s%s%.1f", MICRO_RAM_all_c, difference, (float)RAM_Hz / 1000000);
		
		char remainingBatteryLife[8];
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");

		///Thermal
		if (hosversionAtLeast(10,0,0)) {
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f/%2.1f/%2.1f\u00B0C@%+.1fW[%s]", SOC_temperatureF, PCB_temperatureF, (float)skin_temperaturemiliC / 1000, PowerConsumption, remainingBatteryLife);
		}
		else
			snprintf(skin_temperature_c, sizeof skin_temperature_c, "%2.1f/%2.1f/%2.1f\u00B0C@%+.1fW[%s]", (float)SOC_temperatureC / 1000, (float)PCB_temperatureC / 1000, (float)skin_temperaturemiliC / 1000, PowerConsumption, remainingBatteryLife);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%s", Rotation_SpeedLevel_f * 100, "%");
		
		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%2.1f", FPSavg);

		//Debug
		/*
		snprintf(CPU_compressed_c, sizeof CPU_compressed_c, "[100%s,100%s,100%s,100%s]@1785.0", "%", "%", "%", "%");
		snprintf(GPU_Load_c, sizeof GPU_Load_c, "100.0%s@2400.0", "%");
		snprintf(RAM_var_compressed_c, sizeof RAM_var_compressed_c, "4.4/4.4GB@4444.4");
		snprintf(skin_temperature_c, sizeof skin_temperature_c, "88.8/88.8/88.8\u00B0C@-15.5W[99:99]");
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "100.0%s", "%");
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "60.0");
		*/
		
		
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if ((keysHeld & KEY_LSTICK) && (keysHeld & KEY_RSTICK)) {
			TeslaFPS = 60;
			refreshrate = 60;
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
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
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
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
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
				"Battery Raw Charge: %.1f%s\n"
				"Battery Age: %.1f%s\n"
				"Battery Voltage (5s AVG): %.0f mV\n"
				"Battery Current Flow (5s AVG): %.0f mA\n"
				"Battery Power Flow (5s AVG): %+.3f W\n"
				"Battery Remaining Time: %s",
				actualFullBatCapacity,
				designedFullBatCapacity,
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000, "%",
				(float)_batteryChargeInfoFields.BatteryAge / 1000, "%",
				batVoltageAvg,
				batCurrentAvg,
				PowerConsumption, 
				tempBatTimeEstimate
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
			EndMiscThread();
			nifmExit();
			svcSleepThread(500'000'000);
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Graphs
class GraphsMenu : public tsl::Gui {
public:
    GraphsMenu() { }

    virtual tsl::elm::Element* createUI() override {
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "Graphs");
		auto list = new tsl::elm::List();

		auto comFPSGraph = new tsl::elm::ListItem("FPS");
		comFPSGraph->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				StartFPSCounterThread();
				TeslaFPS = 31;
				refreshrate = 31;
				alphabackground = 0x0;
				tsl::hlp::requestForeground(false);
				FullMode = false;
				tsl::changeTo<com_FPSGraph>();
				return true;
			}
			return false;
		});
		list->addItem(comFPSGraph);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {
		if (TeslaFPS != 60) {
			FullMode = true;
			tsl::hlp::requestForeground(true);
			TeslaFPS = 60;
			alphabackground = 0xD;
			refreshrate = 60;
			systemtickfrequency = 19200000;
		}
	}

    virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			svcSleepThread(300'000'000);
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
		auto rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "Other");
		auto list = new tsl::elm::List();

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

	virtual void update() override {}

    virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysHeld & KEY_B) {
			svcSleepThread(300'000'000);
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
		auto list = new tsl::elm::List(6);
		
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
		if (TeslaFPS != 60) {
			FullMode = true;
			tsl::hlp::requestForeground(true);
			TeslaFPS = 60;
			alphabackground = 0xD;
			refreshrate = 60;
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

			if (R_SUCCEEDED(mmuInitialize())) {
				nvdecCheck = mmuRequestInitialize(&nvdecRequest, MmuModuleId(5), 8, false);
				nvencCheck = mmuRequestInitialize(&nvencRequest, MmuModuleId(6), 8, false);
				nvjpgCheck = mmuRequestInitialize(&nvjpgRequest, MmuModuleId(7), 8, false);
			}

			if (R_SUCCEEDED(audsnoopInitialize())) audsnoopCheck = audsnoopEnableDspUsageMeasurement();

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
				uint32_t api_ver = 0;
				sysclkIpcGetAPIVersion(&api_ver);
				if (api_ver < 3) {
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
		mmuRequestFinalize(&nvdecRequest);
		mmuRequestFinalize(&nvencRequest);
		mmuRequestFinalize(&nvjpgRequest);
		mmuExit();
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
			}
			if (sysclkIpcRunning() && R_SUCCEEDED(sysclkIpcInitialize())) {
				uint32_t api_ver = 0;
				sysclkIpcGetAPIVersion(&api_ver);
				if (api_ver < 3) {
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
	if (argc > 0) {
		filename = argv[0];
	}
	for (u8 arg = 0; arg < argc; arg++) {
		if (strcasecmp(argv[arg], "--microOverlay") == 0) {
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FullMode = false;
			alphabackground = 0x0;
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
