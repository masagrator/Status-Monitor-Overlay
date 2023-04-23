# Status Monitor Overlay
Monitor Your hardware in real time!

This is an overlay homebrew dedicated to Nintendo Switch.
You need to have installed Tesla environment to use it.

Tool contains six modes to choose, each one is explained [here](https://github.com/masagrator/Status-Monitor-Overlay/blob/master/docs/modes.md).

You can exit from selected mode by pressing and holding Left Stick + Right Stick. To exit from main menu press B.

If it's not working in dock, you need to first start Status Monitor, then put Nintendo Switch to dock.

# What is currently supported:
- CPU Usage for each core (Cores `#0`-`#2` are used by apps/games, Core `#3` is used by OS, background processes and also Tesla overlays)
- GPU Load
- CPU, GPU & RAM actual frequency
- Used RAM categorized to: (not supported by FWs <5.0.0)
  - Total
  - Application
  - Applet
  - System
  - System Unsafe
- SoC, PCB & Skin temperatures (Skin temperature not supported by FWs <5.0.0)
- Fan Rotation Level
- PFPS and FPS (with help of [NX-FPS 0.4+](https://github.com/masagrator/NX-FPS), more info in repo. Not installing it results in not showing FPS counters on overlay)
- Battery temperature, raw charge, age, average voltage, average current flow and average power flow
- Charger type, max voltage, and max current
- DSP usage
- NVDEC clock rate
- Network type + Wi-fi password

# Requirements:
- From 0.4.1 you need Tesla Menu >=1.0.2

---

# Thanks to:
- RetroNX channel for helping with coding stuff
- SunTheCourier for sys-clk-Overlay from which I learned how to make my own Tesla homebrew
- Herbaciarz for providing screenshots from HDMI Grabber
- KazushiMe for writing code to read registers from max17050 chip
- CTCaer for Hekate from which I took max17050.h and calculation formulas for reading battery stats from max17050 chip

# FAQ:
Q: This homebrew has any impact on games?

A: Negligible, you won't see any difference. Almost everything is done on Core `#3`, other cores usage is below 0.001%.

# Troubleshooting:

Q: When opening Full or Mini mode, overlay is showing that Core #3 usage is at 100% while everything else is showing 0, eventually leading to crash. Why this happens?

A: There are few possible explanations: 
1. You're using nifm services connection test patches (in short `nifm ctest patches`) that are included in various packs. Those patches allow to connect to network that has no internet connection. But they cause nifm to randomly rampage when connected to network. Find any folder in `atmosphere/exefs_patches` that has in folder name `nifm`, `nfim` and/or `ctest`, delete this folder and restart Switch. If you must use it, only solution is to use this overlay only in airplane mode.
2. You're using some untested custom sysmodule that has no proper thread sleeping implemented. Find out in atmosphere/contents any sysmodule that you don't need, delete it and restart Switch.
3. Your Switch is using sigpatches, is not a primary device, is using linked account, and is connected to network. Delete sigpatches, change your Switch to primary device, unlink account, or disable Wi-Fi. 
