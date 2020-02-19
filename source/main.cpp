#include <tesla.hpp>
#include <switch.h>

u64 systemtickfrequency = 19200000 / 10;
double percent = 0;
u64 idletick_a0 = 0;
u64 idletick_a1 = 0;
u64 idletick_a2 = 0;
u64 idletick_a3 = 0;
u64 idletick_b0 = 0;
u64 idletick_b1 = 0;
u64 idletick_b2 = 0;
u64 idletick_b3 = 0;
u64 idletick0 = 0;
u64 idletick1 = 0;
u64 idletick2 = 0;
u64 idletick3 = 0;
char c_idletick0[32];
char c_idletick1[32];
char c_idletick2[32];
char c_idletick3[32];
char CPU_Usage0[32];
char CPU_Usage1[32];
char CPU_Usage2[32];
char CPU_Usage3[32];
Thread t0;
Thread t1;
Thread t2;
Thread t3;

void CheckCore0() {
	svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
	svcSleepThread(100*1000*1000);
	svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
}

void CheckCore1() {
	svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
	svcSleepThread(100*1000*1000);
	svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
}

void CheckCore2() {
	svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
	svcSleepThread(100*1000*1000);
	svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
}

void CheckCore3() {
	svcGetInfo(&idletick_b3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
	svcSleepThread(100*1000*1000);
	svcGetInfo(&idletick_a3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
}

class GuiMain : public tsl::Gui
{
public:
    GuiMain()
    {
        this->setTitle("Status Monitor");
    }
    ~GuiMain() {}

    // Called when switching Guis to create the new UI
    virtual tsl::Element *createUI()
    {
        tsl::element::Frame *rootFrame = new tsl::element::Frame();
		
        tsl::element::CustomDrawer *Status = new tsl::element::CustomDrawer(0, 0, 100, FB_WIDTH, [](u16 x, u16 y, tsl::Screen *screen) {
			screen->drawString("Used by apps/games:", false, 25, 100, 25, tsl::a(0xFFFF));
			//screen->drawString(c_idletick0, false, 25, 150, 15, tsl::a(0xFFFF));
			//screen->drawString(c_idletick1, false, 25, 165, 15, tsl::a(0xFFFF));
			//screen->drawString(c_idletick2, false, 25, 180, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage0, false, 25, 150, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage1, false, 25, 165, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage2, false, 25, 180, 15, tsl::a(0xFFFF));
			screen->drawString("Used by background processes:", false, 25, 225, 25, tsl::a(0xFFFF));
			//screen->drawString(c_idletick3, false, 25, 275, 15, tsl::a(0xFFFF));
			screen->drawString(CPU_Usage3, false, 25, 275, 15, tsl::a(0xFFFF));
        });

        rootFrame->addElement(Status);

        return rootFrame;
	}
	virtual void update() {	
		threadCreate(&t0, CheckCore0, NULL, NULL, 0x100, 0x3B, 0);
		threadCreate(&t1, CheckCore1, NULL, NULL, 0x100, 0x3B, 1);
		threadCreate(&t2, CheckCore2, NULL, NULL, 0x100, 0x3B, 2);
		threadCreate(&t3, CheckCore3, NULL, NULL, 0x100, 0x3F, 3);
		threadStart(&t0);
		threadStart(&t1);
		threadStart(&t2);
		threadStart(&t3);
		threadWaitForExit(&t0);
		threadWaitForExit(&t1);
		threadWaitForExit(&t2);
		threadWaitForExit(&t3);
		threadClose(&t0);
		threadClose(&t1);
		threadClose(&t2);
		threadClose(&t3);
		idletick0 = idletick_a0 - idletick_b0;
		idletick1 = idletick_a1 - idletick_b1;
		idletick2 = idletick_a2 - idletick_b2;
		idletick3 = idletick_a3 - idletick_b3;
		//snprintf(c_idletick0, sizeof c_idletick0, "Idled ticks Core #0: %u", idletick0);
		//snprintf(c_idletick1, sizeof c_idletick1, "Idled ticks Core #1: %u", idletick1);
		//snprintf(c_idletick2, sizeof c_idletick2, "Idled ticks Core #2: %u", idletick2);
		//snprintf(c_idletick3, sizeof c_idletick3, "Idled ticks Core #3: %u", idletick3);
		if (idletick0 > systemtickfrequency) idletick0 = systemtickfrequency;
		if (idletick1 > systemtickfrequency) idletick1 = systemtickfrequency;
		if (idletick2 > systemtickfrequency) idletick2 = systemtickfrequency;
		if (idletick3 > systemtickfrequency) idletick3 = systemtickfrequency;
		percent = (double) (((double)systemtickfrequency - (double)idletick0) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage0, sizeof CPU_Usage0, "CPU Usage Core #0: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick1) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage1, sizeof CPU_Usage1, "CPU Usage Core #1: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick2) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage2, sizeof CPU_Usage2, "CPU Usage Core #2: %.2f%s", percent, "%");
		percent = (double) (((double)systemtickfrequency - (double)idletick3) / ((double)systemtickfrequency)) * 100;
		snprintf(CPU_Usage3, sizeof CPU_Usage3, "CPU Usage Core #3: %.2f%s", percent, "%");
	}
};

class MonitorOverlay : public tsl::Overlay
{
public:
    MonitorOverlay() {}
    ~MonitorOverlay() {}

    tsl::Gui *onSetup()
    {
        return new GuiMain();
    } // Called once when the Overlay is created and should return the first Gui to load. Initialize services here

    virtual void onDestroy() {} // Called once before the overlay Exits. Exit services here

    virtual void onOverlayShow(tsl::Gui *gui) {}

    void onOverlayHide(tsl::Gui *gui) {}
};

// This function gets called on startup to create a new Overlay object
tsl::Overlay *overlayLoad()
{
    return new MonitorOverlay();
}