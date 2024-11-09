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
private:
	char DSP_Load_c[16];
	char NVDEC_Hz_c[18];
	char NVENC_Hz_c[18];
	char NVJPG_Hz_c[18];
	char Nifm_pass[96];
public:
    MiscOverlay() { 
		smInitialize();
		nifmCheck = nifmInitialize(NifmServiceType_Admin);
		if (R_SUCCEEDED(mmuInitialize())) {
			nvdecCheck = mmuRequestInitialize(&nvdecRequest, MmuModuleId(5), 8, false);
			nvencCheck = mmuRequestInitialize(&nvencRequest, MmuModuleId(6), 8, false);
			nvjpgCheck = mmuRequestInitialize(&nvjpgRequest, MmuModuleId(7), 8, false);
		}

		if (R_SUCCEEDED(audsnoopInitialize())) 
			audsnoopCheck = audsnoopEnableDspUsageMeasurement();

		smExit();
		StartMiscThread();
	}

	~MiscOverlay() {
		EndMiscThread();
		nifmExit();
		mmuRequestFinalize(&nvdecRequest);
		mmuRequestFinalize(&nvencRequest);
		mmuRequestFinalize(&nvjpgRequest);
		mmuExit();
		if (R_SUCCEEDED(audsnoopCheck)) {
			audsnoopDisableDspUsageMeasurement();
		}
		audsnoopExit();
	}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION, true);

		auto Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
			
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

	virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
		if (keysHeld & KEY_Y) {
			Nifm_showpass = true;
		}
		else Nifm_showpass = false;

		if (keysDown & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
	}
};