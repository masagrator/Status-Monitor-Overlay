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
		// APP_VERSION 保持不变
		rootFrame = new tsl::elm::OverlayFrame("状态监视", APP_VERSION "(星野無上)");

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			// "Battery/Charger Stats:" -> "电池/充电统计:"
			renderer->drawString("电池/充电统计:", false, 20, 120, 20, renderer->a(0xFFFF));
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
			// 这里是主要的汉化部分
			snprintf(Battery_c, sizeof Battery_c,
				"电池实际容量: %.0f mAh\n"
				"电池设计容量: %.0f mAh\n"
				"电池温度: %.1f\u00B0C\n"
				"电池原始电量: %.1f%%\n"
				"电池寿命: %.1f%%\n"
				"电池电压 (%d秒平均): %.0f mV\n"
				"电池电流 (%s秒平均): %+.0f mA\n"
				"电池功率%s: %+.3f W\n"
				"电池剩余时间: %s\n"
				"充电器类型: %u\n"
				"充电最大电压: %u mV\n"
				"充电最大电流: %u mA",
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
			// 未连接充电器时的显示
			snprintf(Battery_c, sizeof Battery_c,
				"电池实际容量: %.0f mAh\n"
				"电池设计容量: %.0f mAh\n"
				"电池温度: %.1f\u00B0C\n"
				"电池原始电量: %.1f%%\n"
				"电池寿命: %.1f%%\n"
				"电池电压 (%d秒平均): %.0f mV\n"
				"电池电流 (%s秒平均): %+.0f mA\n"
				"电池功率%s: %+.3f W\n"
				"电池剩余时间: %s",
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