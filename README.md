# Status Monitor Overlay
Monitor Your hardware in real time!

This is an overlay homebrew dedicated to Nintendo Switch.
You need to have installed Tesla environment to use it.

Interval between changes is set to 1s + vsync signal. In case of ~100% core load it can take more time refresh info about this core.

I have changed exit button from B to press and hold Left Stick + Right Stick.

If you want to use it simultaneously with game, you need to first start game, then run this homebrew, next press Home button. Now input is detected by all apps.

If you want to use it in dock, you need to first start Status Monitor, then put Nintendo Switch to dock.

# What is currently showing:
- CPU Usage for each core (Cores #0-#2 are used by apps/games, Core #3 is used by OS, background processes and also Tesla overlays)
- CPU, GPU & RAM actual frequency
- Used RAM categorized to: 
-- Total
-- Application
-- Applet
-- System
-- System Unsafe
- SoC, PCB & Skin temperature
- Fan Rotation Level

# Planned:
- Reduce window size
- Show GPU Usage

![screen image](https://github.com/masagrator/Status-Monitor-Overlay/blob/master/docs/Screen.jpg?raw=true)

---

# Thanks to:
- RetroNX channel for helping with coding stuff
- SunTheCourier for sys-clk-Overlay from which I learned how to make my own Tesla homebrew

# FAQ:
Q: This homebrew has any impact on games?

A: Negligible, you won't see any difference. Almost everything is done on Core #3, other cores usage is below 0.001%.
