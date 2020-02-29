# Status Monitor Overlay
Monitor Your hardware in real time!

This is an overlay homebrew dedicated to Nintendo Switch.
You need to have installed Tesla environment to use it.

Interval between changes is set to 1s + vsync signal by default. You can change that to 200ms + vsync with con of higher Core #3 usage. In case of ~100% core load it can take more time refresh info about this core.

I have changed exit button from B to press and hold Left Stick + Right Stick.

If you want to use it simultaneously with game, you need to first start game, then run this homebrew, next press Home button. Now input is detected by all apps.

If it's not working in dock, you need to first start Status Monitor, then put Nintendo Switch to dock.

# What is currently showing:
- CPU Usage for each core (Cores #0-#2 are used by apps/games, Core #3 is used by OS, background processes and also Tesla overlays)
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
- PFPS and FPS (with help of [NX-FPS](https://github.com/masagrator/NX-FPS), more info in repo. Not installing it results in not showing FPS counters on overlay)

# Planned:
- Add 3 modes: Minimal, Standard, Graph

# Issues:
- Not compatible with Tesla Menu 1.0.2

![screen image](https://github.com/masagrator/Status-Monitor-Overlay/blob/master/docs/screen.jpg?raw=true)

---

# Thanks to:
- RetroNX channel for helping with coding stuff
- SunTheCourier for sys-clk-Overlay from which I learned how to make my own Tesla homebrew
- Herbaciarz for providing screenshots from HDMI Grabber

# FAQ:
Q: This homebrew has any impact on games?

A: Negligible, you won't see any difference. Almost everything is done on Core #3, other cores usage is below 0.001%.

# Troubleshooting:

Q: Game sometimes hangs when using Status Monitor Overlay. Why?

A: This is because of dmnt:cht nature. Some games don't like that it's peaking constantly at its memory and they stuck. This happens only if you are using NX-FPS plugin. Solution is to just close Status Monitor Overlay (you don't need to close Tesla Menu) and wait few seconds until game will resume.
