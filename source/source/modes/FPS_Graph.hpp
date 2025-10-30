class com_FPSGraph : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo); // map buttons
	uint8_t refreshRate = 0;
	char FPSavg_c[8];
	FpsGraphSettings settings;
	uint64_t frametime = 1000000000 / 60;
	int16_t layer_pos_x = 0;
	int16_t layer_pos_x_orig = 0;
	#define layer_pos_x_max 832
	#define layer_pos_x_orig_max 1248
	uint16_t base_x_max = 0;
	uint16_t base_y_max = 0;
	int64_t touch_pos_x = -1;
	int64_t touch_pos_y = -1;
	uint32_t m_base_x = 0;
	uint32_t m_base_y = 0;
	uint32_t m_width = 0;
	uint32_t m_height = 0;
	bool changingPos = false;
	bool changedPos = false;
	bool reachedMaxY = false;
	bool reachedMaxX = false;
public:
	bool isStarted = false;
    com_FPSGraph() { 
		GetConfigSettings(&settings);
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::getRenderer().setLayerPos(624, 0);
				layer_pos_x_orig = 624;
				layer_pos_x = 624 / 3 * 2;
				break;
			case 2:
			case 5:
			case 8:
				tsl::gfx::Renderer::getRenderer().setLayerPos(1248, 0);
				layer_pos_x_orig = 1248;
				layer_pos_x = 1248 / 3 * 2;
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
		frametime = 1000000000 / settings.refreshRate;
		deactivateOriginalFooter = true;
	}

	~com_FPSGraph() {
		EndFPSCounterThread();
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

	s16 rectangle_width = 180;
	s16 rectangle_height = 60;
	s16 rectangle_x = 15;
	s16 rectangle_y = 5;
	s16 rectangle_range_max = 60;
	s16 rectangle_range_min = 0;
	char legend_max[4] = "60";
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
				if (refreshRate < 100) {
					rectangle_x = 15;
					legend_max[0] = 0x30 + (refreshRate / 10);
					legend_max[1] = 0x30 + (refreshRate % 10);
					legend_max[2] = 0;
				}
				else {
					rectangle_x = 22;
					legend_max[0] = 0x30 + (refreshRate / 100);
					legend_max[1] = 0x30 + ((refreshRate / 10) % 10);
					legend_max[2] = 0x30 + (refreshRate % 10);
				}
				y_30FPS = rectangle_y+(rectangle_height / 2);
				range = std::abs(rectangle_range_max - rectangle_range_min) + 1;
			};

			uint32_t width = rectangle_width + ((refreshRate < 100) ? 21 : 28);
			uint32_t height = rectangle_height + 12;

			m_width = width;
			m_height = height;
			base_x_max = 448 - width;
			base_y_max = 720 - height;

			static int base_x = 0;
			static int base_y = 0;
			
			if (!changedPos) switch(settings.setPos) {
				case 0:
					base_x = 0;
					base_y = 0;
					break;
				case 3:
					base_x = 0;
					base_y = 360 - ((rectangle_height + 12) / 2);
					break;
				case 6:
					base_x = 0;
					base_y = 720 - (rectangle_height + 12);
					break;
				case 1:
					base_x = 224 - ((rectangle_width + 21) / 2);
					base_y = 0;
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
					base_y = 0;
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

			if (changingPos) {
				base_x = touch_pos_x - layer_pos_x;
				base_y = touch_pos_y;
			}
			m_base_y = base_y;
			m_base_x = base_x + layer_pos_x;
			if (changingPos) {
				base_x -= (m_width / 2);
				base_y -= (m_height / 2);
				reachedMaxY = false;
				reachedMaxX = false;
			}
			if (base_y >= base_y_max) {
				reachedMaxY = true;
			}
			if (base_x >= base_x_max) {
				reachedMaxX = true;
			}
			if (base_y < 0) {
				base_y = 0;
			}
			if (reachedMaxY) {
				base_y = base_y_max;
			}
			if (reachedMaxX && (layer_pos_x_orig == layer_pos_x_orig_max)) {
				base_x = base_x_max;
			}
			if (base_x < 0) {
				int base_x_abs = std::abs(base_x);
				layer_pos_x_orig -= (base_x_abs * 3) / 2;
				layer_pos_x -= base_x_abs;
				if (layer_pos_x_orig > layer_pos_x_orig_max) layer_pos_x_orig = layer_pos_x_orig_max;
				else if (layer_pos_x_orig < 0) layer_pos_x_orig = 0;
				if (layer_pos_x > layer_pos_x_max) layer_pos_x = layer_pos_x_max;
				else if (layer_pos_x < 0) layer_pos_x = 0;
				base_x = 0;
				tsl::gfx::Renderer::getRenderer().setLayerPos(layer_pos_x_orig, 0);
			}
			else if (base_x > base_x_max) {
				int base_x_abs = base_x - base_x_max;
				layer_pos_x_orig += (base_x_abs * 3) / 2;
				layer_pos_x += base_x_abs;
				if (layer_pos_x_orig > layer_pos_x_orig_max) layer_pos_x_orig = layer_pos_x_orig_max;
				else if (layer_pos_x_orig < 0) layer_pos_x_orig = 0;
				if (layer_pos_x > layer_pos_x_max) layer_pos_x = layer_pos_x_max;
				else if (layer_pos_x < 0) layer_pos_x = 0;
				base_x = base_x_max;
				tsl::gfx::Renderer::getRenderer().setLayerPos(layer_pos_x_orig, 0);
			}

			renderer->drawRect(base_x, base_y, width, height, a(settings.backgroundColor));
			s16 size = (refreshRate > 60 || !refreshRate) ? 63 : (s32)(63.0/(60.0/refreshRate));
			std::pair<u32, u32> dimensions = renderer->drawString(FPSavg_c, false, 0, 0, size, renderer->a(0x0000));
			s16 pos_y = size + base_y + rectangle_y + ((rectangle_height - size) / 2);
			s16 pos_x = base_x + rectangle_x + ((rectangle_width - dimensions.first) / 2);

			renderer->drawString(FPSavg_c, false, pos_x, pos_y, size, renderer->a(settings.fpsColor));
			renderer->drawEmptyRect(base_x+(rectangle_x - 1), base_y+(rectangle_y - 1), rectangle_width + 2, rectangle_height + 4, renderer->a(settings.borderColor));
			renderer->drawDashedLine(base_x+rectangle_x, base_y+y_30FPS, base_x+rectangle_x+rectangle_width, base_y+y_30FPS, 6, renderer->a(settings.dashedLineColor));
			renderer->drawString(&legend_max[0], false, base_x+(rectangle_x-((refreshRate < 100) ? 15 : 22)), base_y+(rectangle_y+7), 10, renderer->a(settings.maxFPSTextColor));
			renderer->drawString(&legend_min[0], false, base_x+(rectangle_x-10), base_y+(rectangle_y+rectangle_height+3), 10, renderer->a(settings.minFPSTextColor));

			size_t last_element = readings.size() - 1;
			
			s16 offset = 0;
			if (refreshRate >= 100) offset = 7;

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

				renderer->drawLine(base_x+x+offset, base_y+y, base_x+x+offset, base_y+y_old, colour);
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
		static uint64_t lastFrame = 0;
		
		uint8_t SaltySharedDisplayRefreshRate = *(uint8_t*)((uintptr_t)shmemGetAddr(&_sharedmemory) + 1);
		if (SaltySharedDisplayRefreshRate) 
			refreshRate = SaltySharedDisplayRefreshRate;
		else refreshRate = 60;
		float m_FPSavg = useOldFPSavg ? FPSavg_old : FPSavg;
		if (m_FPSavg >= 0.f && m_FPSavg <= 1000.f && m_FPSavg != 254.f) {
			snprintf(FPSavg_c, sizeof(FPSavg_c), "%.1f", m_FPSavg);
			if (lastFrame == lastFrameNumber) return;
			else lastFrame = lastFrameNumber;
			if ((s16)(readings.size()) >= rectangle_width) {
				readings.erase(readings.begin());
			}
			float whole = std::round(m_FPSavg);
			temp.value = static_cast<s16>(std::lround(m_FPSavg));
			if (m_FPSavg < whole+0.04 && m_FPSavg > whole-0.05) {
				temp.zero_rounded = true;
			}
			readings.push_back(temp);
		}
		else {
			if (readings.size()) {
				readings.clear();
				readings.shrink_to_fit();
				lastFrame = 0;
			}
			FPSavg_c[0] = 0;
		}
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		bool m_touchScreen = touchScreen;
		if (m_touchScreen) {
			if (*touchInput.delta_time != 0 && (*touchInput.x >= m_base_x && *touchInput.x <= (m_base_x + m_width)) && (*touchInput.y >= m_base_y && *touchInput.y <= (m_base_y + m_height))) {
				changingPos = true;
				changedPos = true;
			}
			else if (changingPos && *touchInput.delta_time == 0) {
				touch_pos_x = -1;
				touch_pos_y = -1;
				changingPos = false;
				return false;
			}
			if (changingPos) {
				touch_pos_x = *touchInput.x;
				touch_pos_y = *touchInput.y;
				if (touch_pos_y >= 704) touch_pos_y = 720;
				else if (touch_pos_y <= 15) touch_pos_y = 0;
				if (touch_pos_x >= 1264) touch_pos_x = 1280;
				else if (touch_pos_x <= 15) touch_pos_x = 0;
			}
		}
		static uint64_t last_time = 0;
		if (!last_time) {
			last_time = armTicksToNs(svcGetSystemTick());
		}
		else if (!changingPos) {
			uint64_t new_time = armTicksToNs(svcGetSystemTick());
			uint64_t delta = new_time - last_time;
			if (delta < frametime) {
				int64_t time_delta = (int64_t)frametime - delta;
				while (time_delta > 1000000) {
					HidTouchScreenState state = {0};
					if (m_touchScreen && hidGetTouchScreenStates(&state, 1) && state.count && (state.touches[0].x >= m_base_x && state.touches[0].x <= (m_base_x + m_width)) && (state.touches[0].y >= m_base_y && state.touches[0].y <= (m_base_y + m_height))) {
						break;
					}
					if (isKeyComboPressed(padGetButtons(&pad), padGetButtonsDown(&pad), mappedButtons)) {
						TeslaFPS = 0;
						tsl::goBack();
						return true;
					}
					svcSleepThread(1000000);
					time_delta -= 1000000;
				}
			}
			last_time = armTicksToNs(svcGetSystemTick());;
		}
		return false;
	}
};