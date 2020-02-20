# Status Monitor Overlay
Monitor Your hardware in real time!

This is an overlay homebrew dedicated to Nintendo Switch.
You need to have installed Tesla environment to use it.

Interval between changes is set to 100 ms. In case of ~100% core load it can take more time.

Because of this you need to hold button instead of just pressing to exit.

I have changed exit button from B to pressing Left Stick.

If you want to use it simultaneously with game, you need to first start game, then run this homebrew, next press Home button. Now input is detected by all apps.

# What is currently showing:
- CPU Usage for each core (Cores #0-#2 are used by apps/games, Core #3 is used by OS and background processes)

# Planned:
- Reduce window size
- Show currenly used clocks
- Show temperatures
- Show used memory
- Show GPU Usage (at this moment I don't have any clue how to do it)

![screen image](https://github.com/masagrator/Status-Monitor-Overlay/blob/master/docs/Screen.jpg?raw=true)

---

# Thanks to:
- RetroNX channel for helping with coding stuff
- SunTheCourier for sys-clk-Overlay from which I learned how to make my own Tesla homebrew

# FAQ:
Q: Is this have impact in game?

A: Negligible, you won't see any difference. You can check in games like Doom and Witcher 3 with enabled FPS counters that running this homebrew doesn't affect FPS in visible way.
