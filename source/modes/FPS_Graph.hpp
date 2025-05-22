class com_FPSGraph : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo); // map buttons
	uint8_t refreshRate = 0;
	char FPSavg_c[8];
	FpsGraphSettings settings;
public:
	bool isStarted = false;
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
		if (R_SUCCEEDED(SaltySD_Connect())) {
			if (R_FAILED(SaltySD_GetDisplayRefreshRate(&refreshRate)))
				refreshRate = 0;
			svcSleepThread(100'000);
			SaltySD_Term();
		}
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		TeslaFPS = settings.refreshRate;
		deactivateOriginalFooter = true;
	}

	~com_FPSGraph() {
		EndFPSCounterThread();
		if (settings.setPos)
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		FullMode = true;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		deactivateOriginalFooter = false;
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

			if (refreshRate && refreshRate < 240) {
				rectangle_height = refreshRate;
				rectangle_range_max = refreshRate;
				legend_max[0] = 0x30 + (refreshRate / 10);
				legend_max[1] = 0x30 + (refreshRate % 10);
				y_30FPS = rectangle_y+(rectangle_height / 2);
				range = std::abs(rectangle_range_max - rectangle_range_min) + 1;
			};
			
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
			s16 size = (refreshRate > 60 || !refreshRate) ? 63 : (s32)(63.0/(60.0/refreshRate));
			std::pair<u32, u32> dimensions = renderer->drawString(FPSavg_c, false, 0, 0, size, renderer->a(0x0000));
			s16 pos_y = size + base_y + rectangle_y + ((rectangle_height - size) / 2);
			s16 pos_x = base_x + rectangle_x + ((rectangle_width - dimensions.first) / 2);

			renderer->drawString(FPSavg_c, false, pos_x, pos_y, size, renderer->a(settings.fpsColor));
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
		stats temp = {0, false};
		static float last = 0;
		
		uint8_t SaltySharedDisplayRefreshRate = *(uint8_t*)((uintptr_t)shmemGetAddr(&_sharedmemory) + 1);
		if (SaltySharedDisplayRefreshRate) 
			refreshRate = SaltySharedDisplayRefreshRate;
		else refreshRate = 60;
		if (FPSavg < 254) {
			if (FPSavg == last) return;
			else last = FPSavg;
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
		else if (readings.size()) {
			readings.clear();
			readings.shrink_to_fit();
			last = 0;
		}
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (isKeyComboPressed(keysHeld, keysDown, mappedButtons)) {
			TeslaFPS = 60;
			tsl::goBack();
			return true;
		}
		return false;
	}
};