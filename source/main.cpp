#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "Utils.hpp"
#include <cstdlib>

static tsl::elm::OverlayFrame* rootFrame = nullptr;
static bool skipMain = false;

#include "modes/FPS_Counter.hpp"
#include "modes/FPS_Graph.hpp"
#include "modes/Full.hpp"
#include "modes/Mini.hpp"
#include "modes/Micro.hpp"
#include "modes/Battery.hpp"
#include "modes/Misc.hpp"
#include "modes/Resolutions.hpp"

//Graphs
class GraphsMenu : public tsl::Gui {
public:
    GraphsMenu() {}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "FPS");
		auto list = new tsl::elm::List();

		auto comFPSGraph = new tsl::elm::ListItem("Graph");
		comFPSGraph->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<com_FPSGraph>();
				return true;
			}
			return false;
		});
		list->addItem(comFPSGraph);

		auto comFPSCounter = new tsl::elm::ListItem("Counter");
		comFPSCounter->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<com_FPS>();
				return true;
			}
			return false;
		});
		list->addItem(comFPSCounter);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysDown & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Other
class OtherMenu : public tsl::Gui {
public:
    OtherMenu() { }

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", "Other");
		auto list = new tsl::elm::List();

		auto Battery = new tsl::elm::ListItem("Battery/Charger");
		Battery->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<BatteryOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Battery);

		auto Misc = new tsl::elm::ListItem("Miscellaneous");
		Misc->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<MiscOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Misc);

		if (SaltySD) {
			auto Res = new tsl::elm::ListItem("Game Resolutions");
			Res->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::changeTo<ResolutionsOverlay>();
					return true;
				}
				return false;
			});
			list->addItem(Res);
		}

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysDown & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
	}
};

//Main Menu
class MainMenu : public tsl::Gui {
public:
    MainMenu() {}

    virtual tsl::elm::Element* createUI() override {
		rootFrame = new tsl::elm::OverlayFrame("Status Monitor", APP_VERSION);
		auto list = new tsl::elm::List();
		
		auto Full = new tsl::elm::ListItem("Full");
		Full->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<FullOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Full);
		auto Mini = new tsl::elm::ListItem("Mini");
		Mini->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<MiniOverlay>();
				return true;
			}
			return false;
		});
		list->addItem(Mini);

		bool fileExist = false;
		FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
		if (test) {
			fclose(test);
			fileExist = true;
			filepath = folderpath + filename;
		}
		else {
			test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
			if (test) {
				fclose(test);
				fileExist = true;
				filepath = folderpath + "Status-Monitor-Overlay.ovl";
			}
		}
		if (fileExist) {
			auto Micro = new tsl::elm::ListItem("Micro");
			Micro->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::setNextOverlay(filepath, "--microOverlay_");
					tsl::Overlay::get()->close();
					return true;
				}
				return false;
			});
			list->addItem(Micro);
		}
		if (SaltySD) {
			auto Graphs = new tsl::elm::ListItem("FPS");
			Graphs->setClickListener([](uint64_t keys) {
				if (keys & KEY_A) {
					tsl::changeTo<GraphsMenu>();
					return true;
				}
				return false;
			});
			list->addItem(Graphs);
		}
		auto Other = new tsl::elm::ListItem("Other");
		Other->setClickListener([](uint64_t keys) {
			if (keys & KEY_A) {
				tsl::changeTo<OtherMenu>();
				return true;
			}
			return false;
		});
		list->addItem(Other);

		rootFrame->setContent(list);

		return rootFrame;
	}

	virtual void update() override {
		if (tsl::cfg::LayerPosX || tsl::cfg::LayerPosY) {
			tsl::gfx::Renderer::getRenderer().setLayerPos(0, 0);
		}
	}

	virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
		if (keysDown & KEY_B) {
			tsl::goBack();
			return true;
		}
		return false;
	}
};

class MonitorOverlay : public tsl::Overlay {
public:

	virtual void initServices() override {
		//Initialize services
		tsl::hlp::doWithSmSession([this]{

			apmInitialize();
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();

			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();

			if (R_SUCCEEDED(fanInitialize())) {
				if (hosversionAtLeast(7,0,0)) fanCheck = fanOpenController(&g_ICon, 0x3D000001);
				else fanCheck = fanOpenController(&g_ICon, 1);
			}

			if (R_SUCCEEDED(nvInitialize())) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");

			psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
			}
			i2cInitialize();

			SaltySD = CheckPort();

			if (SaltySD) {
				LoadSharedMemoryAndRefreshRate();
			}
			if (sysclkIpcRunning() && R_SUCCEEDED(sysclkIpcInitialize())) {
				uint32_t sysClkApiVer = 0;
				sysclkIpcGetAPIVersion(&sysClkApiVer);
				if (sysClkApiVer < 4) {
					sysclkIpcExit();
				}
				else sysclkCheck = 0;
			}
		});
		Hinted = envIsSyscallHinted(0x6F);
	}

	virtual void exitServices() override {
		CloseThreads();
		if (R_SUCCEEDED(sysclkCheck)) {
			sysclkIpcExit();
		}
		shmemClose(&_sharedmemory);
		//Exit services
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanControllerClose(&g_ICon);
		fanExit();
		nvClose(fd);
		nvExit();
		psmExit();
		i2cExit();
		apmExit();
	}

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MainMenu>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

class MicroMode : public tsl::Overlay {
public:

	virtual void initServices() override {
		//Initialize services
		tsl::hlp::doWithSmSession([this]{
			apmInitialize();
			if (hosversionAtLeast(8,0,0)) clkrstCheck = clkrstInitialize();
			else pcvCheck = pcvInitialize();

			if (R_SUCCEEDED(nvInitialize())) nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");

			tsCheck = tsInitialize();
			if (hosversionAtLeast(5,0,0)) tcCheck = tcInitialize();

			if (R_SUCCEEDED(fanInitialize())) {
				if (hosversionAtLeast(7,0,0)) fanCheck = fanOpenController(&g_ICon, 0x3D000001);
				else fanCheck = fanOpenController(&g_ICon, 1);
			}

			i2cInitialize();

			psmCheck = psmInitialize();
			if (R_SUCCEEDED(psmCheck)) {
				psmService = psmGetServiceSession();
			}

			SaltySD = CheckPort();

			if (SaltySD) {
				LoadSharedMemory();
			}
			if (sysclkIpcRunning() && R_SUCCEEDED(sysclkIpcInitialize())) {
				uint32_t sysClkApiVer = 0;
				sysclkIpcGetAPIVersion(&sysClkApiVer);
				if (sysClkApiVer < 4) {
					sysclkIpcExit();
				}
				else sysclkCheck = 0;
			}
		});
		Hinted = envIsSyscallHinted(0x6F);
	}

	virtual void exitServices() override {
		CloseThreads();
		shmemClose(&_sharedmemory);
		if (R_SUCCEEDED(sysclkCheck)) {
			sysclkIpcExit();
		}
		//Exit services
		clkrstExit();
		pcvExit();
		tsExit();
		tcExit();
		fanControllerClose(&g_ICon);
		fanExit();
		i2cExit();
		psmExit();
		nvClose(fd);
		nvExit();
		apmExit();
	}

    virtual void onShow() override {}    // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {}    // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MicroOverlay>();  // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};



// This function gets called on startup to create a new Overlay object
int main(int argc, char **argv) {
	ParseIniFile(); // parse INI from file
    
	if (argc > 0) {
		filename = argv[0];
	}
	for (u8 arg = 0; arg < argc; arg++) {
		if (strcasecmp(argv[arg], "--microOverlay_") == 0) {
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
			if (test) {
				fclose(test);
				filepath = folderpath + filename;
			}
			else {
				test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
				if (test) {
					fclose(test);
					filepath = folderpath + "Status-Monitor-Overlay.ovl";
				}
			}
			return tsl::loop<MicroMode>(argc, argv);
		} else if (strcasecmp(argv[arg], "--microOverlay") == 0) {
            skipMain = true;
			framebufferWidth = 1280;
			framebufferHeight = 28;
			FILE* test = fopen(std::string(folderpath + filename).c_str(), "rb");
			if (test) {
				fclose(test);
				filepath = folderpath + filename;
			}
			else {
				test = fopen(std::string(folderpath + "Status-Monitor-Overlay.ovl").c_str(), "rb");
				if (test) {
					fclose(test);
					filepath = folderpath + "Status-Monitor-Overlay.ovl";
				}
			}
			return tsl::loop<MicroMode>(argc, argv);
		}
	}
    return tsl::loop<MonitorOverlay>(argc, argv);
}
