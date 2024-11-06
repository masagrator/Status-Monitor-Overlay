class MiniOverlay : public tsl::Gui {
private:
	char GPU_Load_c[32] = "";
	char Rotation_SpeedLevel_c[64] = "";
	char RAM_var_compressed_c[128] = "";
	char SoCPCB_temperature_c[64] = "";
	char skin_temperature_c[32] = "";

	uint32_t rectangleWidth = 0;
	char Variables[512] = "";
	size_t fontsize = 0;
	MiniSettings settings;
	bool Initialized = false;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
	uint64_t systemtickfrequency_impl = systemtickfrequency;
public:
    MiniOverlay() { 
		GetConfigSettings(&settings);
		apmGetPerformanceMode(&performanceMode);
		if (performanceMode == ApmPerformanceMode_Normal) {
			fontsize = settings.handheldFontSize;
		}
		else fontsize = settings.dockedFontSize;
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::get().setLayerPos(624, 0);
				break;
			case 2:
			case 5:
			case 8:
				tsl::gfx::Renderer::get().setLayerPos(1248, 0);
				break;
		}
		mutexInit(&mutex_BatteryChecker);
		mutexInit(&mutex_Misc);
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		TeslaFPS = settings.refreshRate;
		systemtickfrequency_impl /= settings.refreshRate;
		deactivateOriginalFooter = true;
        StartThreads();
	}
	~MiniOverlay() {
		CloseThreads();
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		deactivateOriginalFooter = false;
	}

	resolutionCalls m_resolutionRenderCalls[8] = {0};
	resolutionCalls m_resolutionViewportCalls[8] = {0};
	resolutionCalls m_resolutionOutput[8] = {0};
	uint8_t resolutionLookup = 0;
	bool resolutionShow = false;

    virtual tsl::elm::Element* createUI() override {

		rootFrame = new tsl::elm::OverlayFrame("", "");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
		    const u16 frameWidth = 448;  // The width of the frame is 448
		    if (!Initialized) {
		        std::pair<u32, u32> dimensions;
		        rectangleWidth = 0;
		        for (std::string key : ult::split(settings.show, '+')) {
		            if (!key.compare("CPU")) {
		                dimensions = renderer->drawString("[100%,100%,100%,100%]@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));
		                if (rectangleWidth < dimensions.first)
		                    rectangleWidth = dimensions.first;
		            }
		            else if (!key.compare("GPU") || (!key.compare("RAM") && settings.showRAMLoad && R_SUCCEEDED(sysclkCheck))) {
		                dimensions = renderer->drawString("100.0%@4444.4", false, 0, fontsize, fontsize, renderer->a(0x0000));
		                if (rectangleWidth < dimensions.first)
		                    rectangleWidth = dimensions.first;
		            }
		            else if (!key.compare("RAM") && (!settings.showRAMLoad || R_FAILED(sysclkCheck))) {
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
		            else if (!key.compare("RES")) {
		                dimensions = renderer->drawString("3840x2160 || 3840x2160", false, 0, fontsize, fontsize, renderer->a(0x0000));
		                if (rectangleWidth < dimensions.first)
		                    rectangleWidth = dimensions.first;
		            }
		        }
		        Initialized = true;
		    }
		
		    char print_text[36] = "";
		    size_t entry_count = 0;
		    uint8_t flags = 0;
		    for (std::string key : ult::split(settings.show, '+')) {
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
		        else if (!key.compare("RES") && !(flags & 1 << 7) && GameRunning) {
		            if (print_text[0])
		                strcat(print_text, "\n");
		            strcat(print_text, "RES");
		            entry_count++;
		            resolutionShow = true;
		            flags |= (1 << 7);
		        }
		    }
		
		    uint32_t height = (fontsize * entry_count) + (fontsize / 3);
		    uint32_t margin = (fontsize * 4);
		
		    int base_x = 0;
		    int base_y = 0;
		
		    if (ult::useRightAlignment) {
		        base_x = frameWidth - (margin + rectangleWidth + (fontsize / 3));
		    } else {
		        switch (settings.setPos) {
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
		    }
		
		    renderer->drawRect(base_x, base_y, margin + rectangleWidth + (fontsize / 3), height, renderer->a(settings.backgroundColor));
		    renderer->drawString(print_text, false, base_x, base_y + fontsize, fontsize, renderer->a(settings.catColor));
		
		    /// GPU
		    renderer->drawString(Variables, false, base_x + margin, base_y + fontsize, fontsize, renderer->a(settings.textColor));
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
		char MINI_CPU_Usage0[7] = "";
		char MINI_CPU_Usage1[7] = "";
		char MINI_CPU_Usage2[7] = "";
		char MINI_CPU_Usage3[7] = "";

		if (idletick0 > systemtickfrequency_impl)
			strcpy(MINI_CPU_Usage0, "0%");
		else snprintf(MINI_CPU_Usage0, sizeof(MINI_CPU_Usage0), "%.0f%%", (1.d - ((double)idletick0 / systemtickfrequency_impl)) * 100);
		if (idletick1 > systemtickfrequency_impl)
			strcpy(MINI_CPU_Usage1, "0%");
		else snprintf(MINI_CPU_Usage1, sizeof(MINI_CPU_Usage1), "%.0f%%", (1.d - ((double)idletick1 / systemtickfrequency_impl)) * 100);
		if (idletick2 > systemtickfrequency_impl)
			strcpy(MINI_CPU_Usage2, "0%");
		else snprintf(MINI_CPU_Usage2, sizeof(MINI_CPU_Usage2), "%.0f%%", (1.d - ((double)idletick2 / systemtickfrequency_impl)) * 100);
		if (idletick3 > systemtickfrequency_impl)
			strcpy(MINI_CPU_Usage3, "0%");
		else snprintf(MINI_CPU_Usage3, sizeof(MINI_CPU_Usage3), "%.0f%%", (1.d - ((double)idletick3 / systemtickfrequency_impl)) * 100);

		mutexLock(&mutex_Misc);
		
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
		if (R_FAILED(sysclkCheck) || !settings.showRAMLoad) {
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
					ramLoad[SysClkRamLoad_All] / 10, ramLoad[SysClkRamLoad_All] % 10, 
					realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
			}
			else {
				snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c), 
					"%hu.%hhu%%@%hu.%hhu", 
					ramLoad[SysClkRamLoad_All] / 10, ramLoad[SysClkRamLoad_All] % 10, 
					RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
			}
		}
		
		///Thermal
		snprintf(skin_temperature_c, sizeof skin_temperature_c, 
			"%2.1f\u00B0C/%2.1f\u00B0C/%hu.%hhu\u00B0C", 
			SOC_temperatureF, PCB_temperatureF, 
			skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
		snprintf(Rotation_SpeedLevel_c, sizeof Rotation_SpeedLevel_c, "%2.1f%%", Rotation_Duty);

		if (GameRunning && NxFps && resolutionShow) {
			if (!resolutionLookup) {
				(NxFps -> renderCalls[0].calls) = 0xFFFF;
				resolutionLookup = 1;
			}
			else if (resolutionLookup == 1) {
				if ((NxFps -> renderCalls[0].calls) != 0xFFFF) resolutionLookup = 2;
			}
			else {
				memcpy(&m_resolutionRenderCalls, &(NxFps -> renderCalls), sizeof(m_resolutionRenderCalls));
				memcpy(&m_resolutionViewportCalls, &(NxFps -> viewportCalls), sizeof(m_resolutionViewportCalls));
				qsort(m_resolutionRenderCalls, 8, sizeof(resolutionCalls), compare);
				qsort(m_resolutionViewportCalls, 8, sizeof(resolutionCalls), compare);
				memset(&m_resolutionOutput, 0, sizeof(m_resolutionOutput));
				size_t out_iter = 0;
				bool found = false;
				for (size_t i = 0; i < 8; i++) {
					for (size_t x = 0; x < 8; x++) {
						if (m_resolutionRenderCalls[i].width == 0) {
							break;
						}
						if ((m_resolutionRenderCalls[i].width == m_resolutionViewportCalls[x].width) && (m_resolutionRenderCalls[i].height == m_resolutionViewportCalls[x].height)) {
							m_resolutionOutput[out_iter].width = m_resolutionRenderCalls[i].width;
							m_resolutionOutput[out_iter].height = m_resolutionRenderCalls[i].height;
							m_resolutionOutput[out_iter].calls = (m_resolutionRenderCalls[i].calls > m_resolutionViewportCalls[x].calls) ? m_resolutionRenderCalls[i].calls : m_resolutionViewportCalls[x].calls;
							out_iter++;
							found = true;
							break;
						}
					}
					if (!found && m_resolutionRenderCalls[i].width != 0) {
						m_resolutionOutput[out_iter].width = m_resolutionRenderCalls[i].width;
						m_resolutionOutput[out_iter].height = m_resolutionRenderCalls[i].height;
						m_resolutionOutput[out_iter].calls = m_resolutionRenderCalls[i].calls;
						out_iter++;
					}
					found = false;
					if (out_iter == 8) break;
				}
				if (out_iter < 8) {
					size_t out_iter_s = out_iter;
					for (size_t x = 0; x < 8; x++) {
						for (size_t y = 0; y < out_iter_s; y++) {
							if (m_resolutionViewportCalls[x].width == 0) {
								break;
							}
							if ((m_resolutionViewportCalls[x].width == m_resolutionOutput[y].width) && (m_resolutionViewportCalls[x].height == m_resolutionOutput[y].height)) {
								found = true;
								break;
							}
						}
						if (!found && m_resolutionViewportCalls[x].width != 0) {
							m_resolutionOutput[out_iter].width = m_resolutionViewportCalls[x].width;
							m_resolutionOutput[out_iter].height = m_resolutionViewportCalls[x].height;
							m_resolutionOutput[out_iter].calls = m_resolutionViewportCalls[x].calls;
							out_iter++;			
						}
						found = false;
						if (out_iter == 8) break;
					}
				}
				qsort(m_resolutionOutput, 8, sizeof(resolutionCalls), compare);

			}
		}
		else if (!GameRunning && resolutionLookup != 0) {
			resolutionLookup = 0;
		}
		
		///FPS
		char Temp[256] = "";
		uint8_t flags = 0;
		for (std::string key : ult::split(settings.show, '+')) {
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
			else if (!key.compare("RES") && !(flags & 1 << 7) && GameRunning) {
				if (Temp[0]) {
					strcat(Temp, "\n");
				}
				char Temp_s[32] = "";
				if (NxFps -> API == 2) {
					snprintf(Temp_s, sizeof(Temp_s), "EGL");
				}
				else if (NxFps -> API == 3) {
					snprintf(Temp_s, sizeof(Temp_s), "Vulkan");
				}
				else snprintf(Temp_s, sizeof(Temp_s), "%dx%d || %dx%d", m_resolutionOutput[0].width, m_resolutionOutput[0].height, m_resolutionOutput[1].width, m_resolutionOutput[1].height);
				strcat(Temp, Temp_s);
				flags |= 1 << 7;			
			}
		}
		mutexUnlock(&mutex_Misc);
		strcpy(Variables, Temp);

		char remainingBatteryLife[8];
		mutexLock(&mutex_BatteryChecker);
		if (batTimeEstimate >= 0) {
			snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
		}
		else snprintf(remainingBatteryLife, sizeof remainingBatteryLife, "-:--");
		
		snprintf(SoCPCB_temperature_c, sizeof SoCPCB_temperature_c, "%0.2fW[%s]", PowerConsumption, remainingBatteryLife);
		mutexUnlock(&mutex_BatteryChecker);

	}
	virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
		if (isKeyComboPressed(keysHeld, keysDown)) {
			TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		return false;
	}
};