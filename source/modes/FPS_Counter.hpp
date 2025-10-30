class com_FPS : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo); // map buttons
	char FPSavg_c[8];
	FpsCounterSettings settings;
	size_t fontsize = 0;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
	uint64_t frametime = 1000000000 / 60;
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
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		TeslaFPS = settings.refreshRate;
		frametime = 1000000000 / settings.refreshRate;
		deactivateOriginalFooter = true;
		StartFPSCounterThread();
	}
	~com_FPS() {
		EndFPSCounterThread();
		if (settings.setPos)
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		deactivateOriginalFooter = false;
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
		snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f", useOldFPSavg ? FPSavg_old : FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		static uint64_t last_time = 0;
		if (!last_time) {
			last_time = armTicksToNs(svcGetSystemTick());
		}
		else {
			uint64_t new_time = armTicksToNs(svcGetSystemTick());
			uint64_t delta = new_time - last_time;
			if (delta < frametime) {
				uint64_t time_delta = frametime - delta;
				while (time_delta > 1000000) {
					if (isKeyComboPressed(padGetButtons(&pad), padGetButtonsDown(&pad), mappedButtons)) {
						TeslaFPS = 0;
						tsl::goBack();
						return true;
					}
					svcSleepThread(1000000);
					time_delta -= 1000000;
				}
			}
			last_time = new_time;
		}
		return false;
	}
};