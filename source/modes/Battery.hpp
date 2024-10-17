class BatteryOverlay : public tsl::Gui {
private:
	char Battery_c[512];
public:
    BatteryOverlay() {
		mutexInit(&mutex_BatteryChecker);
		StartBatteryThread();
	}
	~BatteryOverlay() {
		CloseThreads();
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION, true);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			renderer->drawString("Battery/Charger Stats:", false, 20, 120, 20, renderer->a(0xFFFF));
			renderer->drawString(Battery_c, false, 20, 155, 18, renderer->a(0xFFFF));
		});

		rootFrame->setContent(Status);

		return rootFrame;
	}

	virtual void update() override {

		///Battery

		mutexLock(&mutex_BatteryChecker);
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
				"Battery Voltage (%ds AVG): %.0f mV\n"
				"Battery Current Flow (%ss AVG): %+.0f mA\n"
				"Battery Power Flow%s: %+.3f W\n"
				"Battery Remaining Time: %s\n"
				"Charger Type: %u\n"
				"Charger Max Voltage: %u mV\n"
				"Charger Max Current: %u mA",
				actualFullBatCapacity,
				designedFullBatCapacity,
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000,
				(float)_batteryChargeInfoFields.BatteryAge / 1000,
				batteryFiltered ? 45 : 5, batVoltageAvg,
				batteryFiltered ? "11.25" : "5", batCurrentAvg,
				batteryFiltered ? "" : " (5s AVG)", PowerConsumption, 
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
				"Battery Voltage (%ds AVG): %.0f mV\n"
				"Battery Current Flow (%ss AVG): %+.0f mA\n"
				"Battery Power Flow%s: %+.3f W\n"
				"Battery Remaining Time: %s",
				actualFullBatCapacity,
				designedFullBatCapacity,
				(float)_batteryChargeInfoFields.BatteryTemperature / 1000,
				(float)_batteryChargeInfoFields.RawBatteryCharge / 1000,
				(float)_batteryChargeInfoFields.BatteryAge / 1000,
				batteryFiltered ? 45 : 5, batVoltageAvg,
				batteryFiltered ? "11.25" : "5", batCurrentAvg,
				batteryFiltered ? "" : " (5s AVG)", PowerConsumption, 
				tempBatTimeEstimate
			);
		mutexUnlock(&mutex_BatteryChecker);
		
	}
	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysDown & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
	}
};
