# Status Monitor Overlay
Monitor Your hardware in real time!

This is an overlay homebrew dedicated to Nintendo Switch.
You need to have installed Tesla environment to use it.

Interval between changes is set to 1s + vsync signal by default. You can change that to 200ms + vsync. In case of ~100% core load it can take more time refresh info about this core.

I have changed exit button from B to press and hold Left Stick + Right Stick.

If you want to use it simultaneously with game, you need to first start game, then run this homebrew, next press Home button. Now input is detected by all apps.

If it's not working in dock, you need to first start Status Monitor, then put Nintendo Switch to dock.

# What is currently showing:
- CPU Usage for each core (Cores #0-#2 are used by apps/games, Core #3 is used by OS, background processes and also Tesla overlays)
- GPU Load
- CPU, GPU & RAM actual frequency
- Used RAM categorized to: 
  - Total
  - Application
  - Applet
  - System
  - System Unsafe
- SoC, PCB & Skin temperatures
- Fan Rotation Level

# Planned:
- Add 3 modes: Minimal, Standard, Graph
- Add FPS Counter

![screen image](https://github.com/masagrator/Status-Monitor-Overlay/blob/master/docs/screen.jpg?raw=true)

---

# Thanks to:
- RetroNX channel for helping with coding stuff
- SunTheCourier for sys-clk-Overlay from which I learned how to make my own Tesla homebrew
- Herbaciarz for providing screenshots from HDMI Grabber

# FAQ:
Q: This homebrew has any impact on games?

A: Negligible, you won't see any difference. Almost everything is done on Core #3, other cores usage is below 0.001%.
