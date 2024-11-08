class MicroOverlay : public tsl::Gui {
private:
	char GPU_Load_c[32] = "";
	char Rotation_SpeedLevel_c[64] = "";
	char RAM_var_compressed_c[128] = "";
	char CPU_compressed_c[160] = "";
	char CPU_Usage0[32] = "";
	char CPU_Usage1[32] = "";
	char CPU_Usage2[32] = "";
	char CPU_Usage3[32] = "";
	char skin_temperature_c[32] = "";
	char batteryCharge[10] = ""; // Declare the batteryCharge variable
	char FPS_var_compressed_c[64] = "";

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
	uint64_t systemtickfrequency_impl = systemtickfrequency;
public:
    MicroOverlay() { 
		GetConfigSettings(&settings);
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else fontsize = settings.dockedFontSize;
		if (settings.setPosBottom) {
			//auto [horizontalUnderscanPixels, verticalUnderscanPixels] = tsl::gfx::getUnderscanPixels();
			//tsl::gfx::Renderer::get().setLayerPos(0, 1038-verticalUnderscanPixels);
			tsl::gfx::Renderer::get().setLayerPos(0, 1038);
		}
		mutexInit(&mutex_BatteryChecker);
		mutexInit(&mutex_Misc);
		TeslaFPS = settings.refreshRate;
		systemtickfrequency_impl /= settings.refreshRate;
		alphabackground = 0x0;
		deactivateOriginalFooter = true;
        StartThreads();
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
				if (R_FAILED(sysclkCheck) || !settings.showRAMLoad) {
					RAM_dimensions = renderer->drawString("RAM 4.4/44.4GB△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				}
				else RAM_dimensions = renderer->drawString("RAM 100.0%△4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				BRD_dimensions = renderer->drawString("BRD 88.8/88.8/88.8\u00B0C@-15.5W[99:99]", false, 0, fontsize, fontsize, renderer->a(0x0000));
				FAN_dimensions = renderer->drawString("FAN 100.0%", false, 0, fontsize, fontsize, renderer->a(0x0000));
				FPS_dimensions = renderer->drawString("FPS 44.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
				auto spacesize = renderer->drawString(" ", false, 0, fontsize, fontsize, renderer->a(0x0000));
				margin = spacesize.first;
				text_width = 0;
				int8_t entry_count = -1;
				uint8_t flags = 0;
				for (std::string key : ult::split(settings.show, '+')) {
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
				tsl::hlp::requestForeground(false);
			}

			u32 base_y = 3;
			if (settings.setPosBottom) {
				base_y = tsl::cfg::FramebufferHeight - (fontsize + (fontsize / 4));
			}

			//renderer->drawRect(0, base_y, tsl::cfg::FramebufferWidth, fontsize + (fontsize / 4), a(settings.backgroundColor));

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
			for (std::string key : ult::split(settings.show, '+')) {
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
			if (fontsize != settings.handheldFontSize) {
				Initialized = false;
				fontsize = settings.handheldFontSize;
			}
		}
		else if (performanceMode == ApmPerformanceMode_Boost) {
			if (fontsize != settings.dockedFontSize) {
				Initialized = false;
				fontsize = settings.dockedFontSize;
			}
		}

		//Make stuff ready to print
		///CPU
		if (idletick0 > systemtickfrequency_impl)
			strcpy(CPU_Usage0, "0%");
		else snprintf(CPU_Usage0, sizeof CPU_Usage0, "%.0f%%", (1.d - ((double)idletick0 / systemtickfrequency_impl)) * 100);
		if (idletick1 > systemtickfrequency_impl)
			strcpy(CPU_Usage1, "0%");
		else snprintf(CPU_Usage1, sizeof CPU_Usage1, "%.0f%%", (1.d - ((double)idletick1 / systemtickfrequency_impl)) * 100);
		if (idletick2 > systemtickfrequency_impl)
			strcpy(CPU_Usage2, "0%");
		else snprintf(CPU_Usage2, sizeof CPU_Usage2, "%.0f%%", (1.d - ((double)idletick2 / systemtickfrequency_impl)) * 100);
		if (idletick3 > systemtickfrequency_impl)
			strcpy(CPU_Usage3, "0%");
		else snprintf(CPU_Usage3, sizeof CPU_Usage3, "%.0f%%", (1.d - ((double)idletick3 / systemtickfrequency_impl)) * 100);

		mutexLock(&mutex_Misc);
		char difference[5] = "@";
		if (realCPU_Hz) {
			int32_t deltaCPU = (int32_t)(realCPU_Hz / 1000) - (CPU_Hz / 1000);
			if (deltaCPU > 20000) {
				strcpy(difference, "△");
			}
			else if (deltaCPU < -50000) {
				strcpy(difference, "≠");
			}
			else if (deltaCPU < -20000) {
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
			int32_t deltaGPU = (int32_t)(realGPU_Hz / 1000) - (GPU_Hz / 1000);
			if (deltaGPU >= 20000) {
				strcpy(difference, "△");
			}
			else if (deltaGPU > -20000) {
				strcpy(difference, "@");
			}
			else if (deltaGPU < -50000) {
				strcpy(difference, "≠");
			}
			else if (deltaGPU < -20000) {
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
		if (!settings.showRAMLoad || R_FAILED(sysclkCheck)) {
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
			snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%hu.%hhu%%", ramLoad[SysClkRamLoad_All] / 10, ramLoad[SysClkRamLoad_All] % 10);
		}

		if (realRAM_Hz) {
			int32_t deltaRAM = (int32_t)(realRAM_Hz / 1000) - (RAM_Hz / 1000);
			if (deltaRAM >= 20000) {
				strcpy(difference, "△");
			}
			else if (deltaRAM > -20000) {
				strcpy(difference, "@");
			}
			else if (deltaRAM < -50000) {
				strcpy(difference, "≠");
			}
			else if (deltaRAM < -20000) {
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
		mutexLock(&mutex_BatteryChecker);
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");

		///Thermal
		snprintf(skin_temperature_c, sizeof skin_temperature_c, 
			"%2.1f/%2.1f/%hu.%hhu\u00B0C@%+.1fW[%s]", 
			SOC_temperatureF, PCB_temperatureF, 
			skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10, 
			PowerConsumption, remainingBatteryLife);
		mutexUnlock(&mutex_BatteryChecker);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%%", Rotation_Duty);
		
		///FPS
		snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%2.1f", FPSavg);

		mutexUnlock(&mutex_Misc);
		
		
		
	}
	virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
		if (isKeyComboPressed(keysHeld, keysDown)) {
			TeslaFPS = 60;
            if (skipMain)
                tsl::goBack();
            else {
			    tsl::setNextOverlay(filepath.c_str());
			    tsl::Overlay::get()->close();
            }
			return true;
		}
		return false;
	}
};