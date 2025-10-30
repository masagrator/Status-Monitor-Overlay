class ResolutionsOverlay : public tsl::Gui {
private:
	uint64_t mappedButtons = MapButtons(keyCombo);
	char Resolutions_c[512];
	char Resolutions2_c[512];
	ResolutionSettings settings;
	uint64_t frametime = 1000000000 / 60;
	int16_t layer_pos_x = 0;
	int16_t layer_pos_x_orig = 0;
	#define layer_pos_x_max 832
	#define layer_pos_x_orig_max 1248
	int64_t touch_pos_x = -1;
	int64_t touch_pos_y = -1;
	uint32_t m_base_x = 0;
	uint32_t m_base_y = 0;
	#define m_width 360
	uint32_t m_height = 0;
	#define base_x_max (448 - m_width)
	uint16_t base_y_max = 0;
	bool changingPos = false;
	bool changedPos = false;
	bool reachedMaxY = false;
	bool reachedMaxX = false;
public:
    ResolutionsOverlay() {
		GetConfigSettings(&settings);
		switch(settings.setPos) {
			case 1:
			case 4:
			case 7:
				tsl::gfx::Renderer::getRenderer().setLayerPos(639, 0);
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
		deactivateOriginalFooter = true;
		tsl::hlp::requestForeground(false);
		alphabackground = 0x0;
		FullMode = false;
		TeslaFPS = settings.refreshRate;
		frametime = 1000000000 / settings.refreshRate;
	}
	~ResolutionsOverlay() {
		EndFPSCounterThread();
		deactivateOriginalFooter = false;
		tsl::hlp::requestForeground(true);
		alphabackground = 0xD;
		FullMode = true;
	}

	resolutionCalls m_resolutionRenderCalls[8] = {0};
	resolutionCalls m_resolutionViewportCalls[8] = {0};
	bool gameStart = false;
	uint8_t resolutionLookup = 0;

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("", "");
		
		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
			static int base_y = 0;
			static int base_x = 0;
			if (!changedPos) switch(settings.setPos) {
				case 0:
					base_x = 0;
					base_y = 0;
					break;
				case 1:
					base_x = 48;
					base_y = 0;
					break;
				case 2:
					base_x = 96;
					base_y = 0;
					break;
				case 3:
					base_x = 0;
					base_y = 260;
					break;
				case 4:
					base_x = 48;
					base_y = 260;
					break;
				case 5:
					base_x = 96;
					base_y = 260;
					break;
				case 6:
					base_x = 0;
					base_y = 520;
					break;
				case 7:
					base_x = 48;
					base_y = 520;
					break;
				case 8:
					base_x = 96;
					base_y = 520;
					break;	
			}
			
			bool ready = false;
			if (gameStart && NxFps -> API >= 1) {
				ready = true;
			}
			if (ready) {
				m_height = 200;
			}
			else {
				m_height = 28;
			}

			base_y_max = 720 - m_height;

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

			if (ready) {
				renderer->drawRect(base_x, base_y, m_width, m_height, a(settings.backgroundColor));

				renderer->drawString("Depth:", false, base_x+20, base_y+20, 20, renderer->a(settings.catColor));
				renderer->drawString(Resolutions_c, false, base_x+20, base_y+55, 18, renderer->a(settings.textColor));
				renderer->drawString("Viewport:", false, base_x+180, base_y+20, 20, renderer->a(settings.catColor));
				renderer->drawString(Resolutions2_c, false, base_x+180, base_y+55, 18, renderer->a(settings.textColor));
			}
			else {
				if (!changedPos) switch(settings.setPos) {
					case 3 ... 5:
						base_y = 346;
						break;
					case 6 ... 8:
						base_y = 692;
						break;
				}
				renderer->drawRect(base_x, base_y, m_width, 28, a(settings.backgroundColor));
				renderer->drawString("Game is not running or it's incompatible.", false, base_x, base_y+20, 18, renderer->a(0xF00F));
			}
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {

		if (gameStart && NxFps) {
			if (!resolutionLookup) {
				NxFps -> renderCalls[0].calls = 0xFFFF;
				resolutionLookup = 1;
			}
			else if (resolutionLookup == 1) {
				if ((NxFps -> renderCalls[0].calls) != 0xFFFF) resolutionLookup = 2;
				else return;
			}
			memcpy(&m_resolutionRenderCalls, &(NxFps -> renderCalls), sizeof(m_resolutionRenderCalls));
			memcpy(&m_resolutionViewportCalls, &(NxFps -> viewportCalls), sizeof(m_resolutionViewportCalls));
			qsort(m_resolutionRenderCalls, 8, sizeof(resolutionCalls), compare);
			qsort(m_resolutionViewportCalls, 8, sizeof(resolutionCalls), compare);
			snprintf(Resolutions_c, sizeof Resolutions_c,
				"1. %dx%d, %d\n"
				"2. %dx%d, %d\n"
				"3. %dx%d, %d\n"
				"4. %dx%d, %d\n"
				"5. %dx%d, %d\n"
				"6. %dx%d, %d\n"
				"7. %dx%d, %d\n"
				"8. %dx%d, %d",
				m_resolutionRenderCalls[0].width, m_resolutionRenderCalls[0].height, m_resolutionRenderCalls[0].calls,
				m_resolutionRenderCalls[1].width, m_resolutionRenderCalls[1].height, m_resolutionRenderCalls[1].calls,
				m_resolutionRenderCalls[2].width, m_resolutionRenderCalls[2].height, m_resolutionRenderCalls[2].calls,
				m_resolutionRenderCalls[3].width, m_resolutionRenderCalls[3].height, m_resolutionRenderCalls[3].calls,
				m_resolutionRenderCalls[4].width, m_resolutionRenderCalls[4].height, m_resolutionRenderCalls[4].calls,
				m_resolutionRenderCalls[5].width, m_resolutionRenderCalls[5].height, m_resolutionRenderCalls[5].calls,
				m_resolutionRenderCalls[6].width, m_resolutionRenderCalls[6].height, m_resolutionRenderCalls[6].calls,
				m_resolutionRenderCalls[7].width, m_resolutionRenderCalls[7].height, m_resolutionRenderCalls[7].calls
			);
			snprintf(Resolutions2_c, sizeof Resolutions2_c,
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d\n"
				"%dx%d, %d",
				m_resolutionViewportCalls[0].width, m_resolutionViewportCalls[0].height, m_resolutionViewportCalls[0].calls,
				m_resolutionViewportCalls[1].width, m_resolutionViewportCalls[1].height, m_resolutionViewportCalls[1].calls,
				m_resolutionViewportCalls[2].width, m_resolutionViewportCalls[2].height, m_resolutionViewportCalls[2].calls,
				m_resolutionViewportCalls[3].width, m_resolutionViewportCalls[3].height, m_resolutionViewportCalls[3].calls,
				m_resolutionViewportCalls[4].width, m_resolutionViewportCalls[4].height, m_resolutionViewportCalls[4].calls,
				m_resolutionViewportCalls[5].width, m_resolutionViewportCalls[5].height, m_resolutionViewportCalls[5].calls,
				m_resolutionViewportCalls[6].width, m_resolutionViewportCalls[6].height, m_resolutionViewportCalls[6].calls,
				m_resolutionViewportCalls[7].width, m_resolutionViewportCalls[7].height, m_resolutionViewportCalls[7].calls
			);
		}
		if (FPSavg < 254) {
			gameStart = true;
		}
		else {
			gameStart = false;
			resolutionLookup = false;
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
				uint64_t time_delta = frametime - delta;
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
			last_time = armTicksToNs(svcGetSystemTick());
		}
		return false;
	}
};
