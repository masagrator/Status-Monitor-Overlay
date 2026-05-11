class com_FPS : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo); // map buttons
	uint64_t leftJoyconMotionMappedButtons = MapButtons(leftJoyconMotionKeyCombo);
	uint64_t rightJoyconMotionMappedButtons = MapButtons(rightJoyconMotionKeyCombo);
	uint64_t proControllerMotionMappedButtons = MapButtons(proControllerMotionKeyCombo);
	char FPSavg_c[8];
	FpsCounterSettings settings;
	size_t fontsize = 0;
	ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
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
	bool sixaxisChangingPos = false;
	bool changedPos = false;
	bool reachedMaxY = false;
	bool reachedMaxX = false;
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
		alphabackground = 0x0;
		tsl::hlp::requestForeground(false);
		FullMode = false;
		TeslaFPS = settings.refreshRate;
		frametime = 1000000000 / settings.refreshRate;
		deactivateOriginalFooter = true;
		StartFPSCounterThread();
		if (motionControl == true) {
			hidsysSetAppletResourceUserId();
			hidStartSixAxisSensor(sixaxisHandles[Controller_ProController]);
			hidStartSixAxisSensor(sixaxisHandles[Controller_JoyConL]);
			hidStartSixAxisSensor(sixaxisHandles[Controller_JoyConR]);
		}
	}
	~com_FPS() {
		if (motionControl == true) {
			hidStopSixAxisSensor(sixaxisHandles[Controller_ProController]);
			hidStopSixAxisSensor(sixaxisHandles[Controller_JoyConL]);
			hidStopSixAxisSensor(sixaxisHandles[Controller_JoyConR]);
		}
		EndFPSCounterThread();
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

			uint32_t width = rectangleWidth + margin;
			uint32_t height = fontsize + (margin / 2);

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
					base_y = 360 - ((fontsize + (margin / 2)) / 2);
					break;
				case 6:
					base_x = 0;
					base_y = 720 - (fontsize + (margin / 2));
					break;
				case 1:
					base_x = 224 - ((rectangleWidth + margin) / 2);
					base_y = 0;
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
					base_y = 0;
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
		float m_FPSavg = useOldFPSavg ? FPSavg_old : FPSavg;
		if (m_FPSavg <= 0.f || m_FPSavg >= 1000.f || m_FPSavg == 254.f) {
			strcpy(FPSavg_c, "n/d");
		}
		else snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f", m_FPSavg);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		bool m_touchScreen = touchScreen;
		if (__builtin_expect(m_touchScreen && (sixaxisChangingPos == false), false)) {
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
		if (changingPos == false || sixaxisChangingPos == true) {
			HidSixAxisSensorState sixaxis = {0};
			static bool start = false;
			s32 id = -1;
			u64 style_set = padGetStyleSet(&pad);
			if (style_set & HidNpadStyleTag_NpadJoyDual) {
				if ((keysHeld & leftJoyconMotionMappedButtons) == leftJoyconMotionMappedButtons) id = Controller_JoyConL;
				else if ((keysHeld & rightJoyconMotionMappedButtons) == rightJoyconMotionMappedButtons) id = Controller_JoyConR;
			}
			else if (style_set & HidNpadStyleTag_NpadJoyLeft) {
				if ((keysHeld & leftJoyconMotionMappedButtons) == leftJoyconMotionMappedButtons) id = Controller_JoyConL;
			}
			else if (style_set & HidNpadStyleTag_NpadJoyRight) {
				if ((keysHeld & rightJoyconMotionMappedButtons) == rightJoyconMotionMappedButtons) id = Controller_JoyConR;
			}
			else if (style_set & HidNpadStyleTag_NpadJoyRight) {
				if ((keysHeld & proControllerMotionMappedButtons) == proControllerMotionMappedButtons) id = Controller_ProController;
			}
			if (id < 0) {
				start = false;
				changingPos = false;
				sixaxisChangingPos = false;
			}
			else {
				static GyroCursor cursor;
				hidGetSixAxisSensorStates(sixaxisHandles[id], &sixaxis, 1);
				if (sixaxis.acceleration.x == 0.f && sixaxis.acceleration.y == 0.f && sixaxis.acceleration.z == -1.f) {
					hidsysSetAppletResourceUserId();
					hidGetSixAxisSensorStates(sixaxisHandles[id], &sixaxis, 1);
				}
				if (sixaxis.acceleration.x != 0.f || sixaxis.acceleration.y != 0.f || sixaxis.acceleration.z != -1.f) {
					if (start == false) {
						start = true;
						float sensitivity = 200;
						gyroCursor_init(cursor, (float)m_base_x, (float)m_base_y, sensitivity);
					}
					changingPos = true;
					changedPos = true;
					sixaxisChangingPos = true;
					gyroCursor_update(cursor, sixaxis, &touch_pos_x, &touch_pos_y);
					
				}
				else {
					start = false;
					changingPos = false;
					sixaxisChangingPos = false;
				}
			}
		}
		static uint64_t last_time = 0;
		if (__builtin_expect(!last_time, 0)) {
			last_time = armTicksToNs(svcGetSystemTick());
		}
		else if (!changingPos) {
			uint64_t new_time = armTicksToNs(svcGetSystemTick());
			uint64_t delta = new_time - last_time;
			if (delta < frametime) {
				uint64_t time_delta = frametime - delta;
				while (time_delta > 1000000) {
					HidTouchScreenState state = {0};
					padUpdate(&pad);
					keysHeld = padGetButtons(&pad);
					keysDown = padGetButtonsDown(&pad);
					if (m_touchScreen && hidGetTouchScreenStates(&state, 1) && state.count && (state.touches[0].x >= m_base_x && state.touches[0].x <= (m_base_x + m_width)) && (state.touches[0].y >= m_base_y && state.touches[0].y <= (m_base_y + m_height))) {
						break;
					}
					u64 style_set = padGetStyleSet(&pad);
					if (style_set & HidNpadStyleTag_NpadJoyDual) {
						if ((keysHeld & leftJoyconMotionMappedButtons) == leftJoyconMotionMappedButtons) break;
						else if ((keysHeld & rightJoyconMotionMappedButtons) == rightJoyconMotionMappedButtons) break;
					}
					else if (style_set & HidNpadStyleTag_NpadJoyLeft) {
						if ((keysHeld & leftJoyconMotionMappedButtons) == leftJoyconMotionMappedButtons) break;
					}
					else if (style_set & HidNpadStyleTag_NpadJoyRight) {
						if ((keysHeld & rightJoyconMotionMappedButtons) == rightJoyconMotionMappedButtons) break;
					}
					else if (style_set & HidNpadStyleTag_NpadJoyRight) {
						if ((keysHeld & proControllerMotionMappedButtons) == proControllerMotionMappedButtons) break;
					}
					if (__builtin_expect(isKeyComboPressed(keysHeld, keysDown, mappedButtons), false)) {
						TeslaFPS = 0;
						tsl::goBack();
						return true;
					}
					svcSleepThread(1000000);
					time_delta -= 1000000;
				}
			}
			last_time = armTicksToNs(svcGetSystemTick());
		}
		return false;
	}
};
