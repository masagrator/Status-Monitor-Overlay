# Modes

Status Monitor Overlay from 1.0.0 release contains five modes to choose from Main Menu.<br>
For additional functions you need to install:
- [SaltyNX](https://github.com/masagrator/SaltyNX/releases)
- [sys-clk 2.0.0_rc4+](https://github.com/retronx-team/sys-clk/releases) (using closed source forks of sys-clk can result in retrieving wrong real clockrates and ram load)

# Full

This mode you can know from older releases of Status Monitor. It contains all informations properly described and supported with high precision.

- CPU Usage 
  - Real Frequency: `%.1f` = Real clockrate of all CPU cores in MHz (This shows only when sys-clk 2.0.0_4c+ is installed)
  - Target Frequency: `%.1f` = Target clockrate of all CPU cores in MHz
  - Core #0-#3: `%.2f` = Load of CPU Cores calculated from IdleTickCount to percent value

- GPU Usage
  - Real Frequency: `%.1f` = Real clockrate of GPU in MHz (This shows only when sys-clk 2.0.0_rc4+ is installed)
  - Target Frequency: `%.1f` = Target clockrate of GPU in MHz
  - Load: `%.1f` = GPU Load provided by PMU in percent

- RAM Usage
  - Real Frequency: `%.1f` = Real clockrate of EMC in MHz (This shows only when sys-clk 2.0.0_rc4+ is installed)
  - Target Frequency: `%.1f` = Target clockrate of EMC in MHz
  - Load: `%.1f` (CPU `%.1f` | GPU `%.1f`) = RAM Load in % (Which part of that are CPU and GPU (with other hardware, but their impact on GPU readings is negligible))
  - `%s`: `%.2f`/`%.2f` = %s memory used/%s memory available in MB (not working with FW <5.0.0)

- Board
  - Battery Power Flow: `%+.2f`[h:mm] = How much power in watts is discharged from or charged to the battery [time left before shutdown]
  - Temperatures:  SoC: `%.1f` / PCB: `%.1f` / Skin: `%.1f` = SoC / PCB / Skin temperature in Celsius degrees (Explanation provided at the end of file)
  - Fan Rotation Level: `%.1f` = Fan rotation level in percent


```Optional (shows only when SaltyNX is installed and game is running)```

| Category | Format            | Explanation                                                              |
|----------|-------------------|--------------------------------------------------------------------------|
| PFPS     | %u                | Pushed Frames Per Second - how many frames were displayed in last second |
| FPS      | %.2f              | Frames Per Second - value calculated by averaging frametime              |
| Resolutions | %dx%d \|\| %dx%d | Two the most promising candidates for internal game resolution `(^1)`    |

- ^1 - read more informations in explanation for "Game Resolutions" menu

# Mini

Contains most of supported informations with lower precision.

| Category | Format                                           | Explanation                                                               |
|----------|--------------------------------------------------|---------------------------------------------------------------------------|
| CPU      | [%.0f,%.0f,%.0f,%.0f]@%.1f                       | Core #0 usage, Core #1 usage, Core #2 usage, Core #3 usage@CPU Target/Real frequency`(^1)`  |
| GPU      | %.1f@%.1f                                        | Load@GPU Target/Real Frequency`(^1)`                                      |
| RAM      | %.0f/%.0f@%.1f `or` %.1f@%.1f                    | Total RAM used/Total RAM available in MB@EMC Target/Real frequency `or` RAM load@EMC Target frequency `(^1)`  |
| TEMP     | %2.1f/%2.1f/%2.1f                                | SoC temperature/PCB temperature/Skin temperature `(^2)`                   |
| FAN      | %2.1f                                            | Fan rotation level                                                        |
| DRAW     | %+.2f[h:mm]                                      | How much power in watts is discharged from or charged to the battery [Time left before shutdown]      |
| RES      | %dx%d \|\| %dx%d                                   | Two the most promising candidates for internal game resolution `(^3)`     |

- ^1 - Real Frequency + RAM Load available only with sys-clk 2.0.0_rc4+
- ^2 - Explanation provided at the end of file
- ^3 - read more informations in explanation for "Game Resolutions" menu

```Optional```

> shows only when SaltyNX is installed and game is running
> 
| Category | Format            | Explanation                                                              |
|----------|-------------------|--------------------------------------------------------------------------|
| FPS      | %.2f              | Frames Per Second                                                        |

# Micro

Contains most of supported informations with lower precision in one line.

| Category | Format                                                     | Explanation                                                               |
|----------|------------------------------------------------------------|---------------------------------------------------------------------------|
| CPU      | [%.0f,%.0f,%.0f,%.0f]%s%.1f                                 | [Core #0 usage, Core #1 usage, Core #2 usage, Core #3 usage] `(^1)` CPU Target/Real Frequency `(^3)`  |
| GPU      | %.1f%s%.1f                                                  | Load (^1) GPU Target/Real Frequency                                      |
| RAM      | %.1f/%.1f%s%.1f `or` %.1f%s%.1f                             | Total RAM used/Total RAM available in GB `(^1)` EMC Target frequency `or` RAM Load in % `(^1)` EMC Target frequency      |
| BRD      | %2.1f/%2.1f/%2.1f@+.1f[h:mm]                                | SoC temperature/PCB temperature/Skin temperature(^2)/Battery Power Flow[Time left before shutdown]      |
| FAN      | %2.1f                                                       | Fan rotation level                                                               |

- ^1 - by default it's `@`, but if you have sys-clk 2.0.0_rc4+ installed, this changes depending on difference between real clocks and targeted clocks. <br>
  - `△` - real clocks are higher by at least 20 MHz than expected
  - `▽` - real clocks are lower  by at least 20 MHz than expected
  - `≠` - real clocks are lower by at least 50 MHz than expected (If this shows constantly for longer than few seconds, this is a sign of throttling, usually caused by bad overclocking configuration)
- ^2 - Explanation provided at the end of file
- ^3 - Real frequencies + RAM Load available only with sys-clk 2.0.0_rc4+

```Optional```

> shows only when SaltyNX is installed and game is running

| Category | Format             | Explanation                                                              |
|----------|--------------------|--------------------------------------------------------------------------|
| FPS      | %.1f               | Frames Per Second                                                        |


# FPS 

> Counter

It shows only FPS value in 31Hz + vsync signal. <br>
If game is not launched, it will show always 254.0 value.<br>

Mode available only with SaltyNX installed.

> Graph

It shows average FPS graph in 31Hz + vsync signal. In background of graph you can see rendered actual average FPS.<br>
If game is not launched, it will show always 254.0 value and graph will be empty.<br>
If game is not rendering any new frame, graph is not updated.

If line is within rounding error of graph line, it's colored. Average FPS must be between -0.05 and +0.04 of graph line target FPS. So for 30 FPS it's between 29.95 - 30.04 FPS. For 43 FPS it's 42.95 - 43.04 FPS.
If it's in display refresh rate or half of it rounding error range, it's green. Any other FPS graph target is purple.

Colors can be changed in config file.

Mode available only with SaltyNX installed.

# Other

> Battery

- Battery Actual Capacity: `%d` = Battery Designed Capacity multiplied by Battery Age in mAh
- Battery Designed Capacity: `%d` = Battery capacity targeted by manufacturer in mAh
- Battery Temperature: `%.1f` = Battery temperature in Celsius
- Battery Raw Charge: `%.1f` = Raw battery charged capacity in percent
- Battery Age: `%.1f` = How much of designed capacity was charged last time battery was charged completely in percent
- Battery Voltage: `%.0f` = Battery average voltage in mV (time period: 5s, or with `battery_avg_iir_filter` enabled: 45s)
- Battery Current Flow: `%+.0f` = Battery average current flow in mA (time period: 5s, or with `battery_avg_iir_filter` enabled: 11.25s)
- Battery Power Flow: `%+.3f` = Battery average power flow in W calculated from Battery Voltage and Battery Current Flow
- Battery Remaining Time: h:mm - How much time is left before shutdown

Shows only if charger is connected:
| Category                  | Format       | Explanation                                                                      |
|---------------------------|--------------|----------------------------------------------------------------------------------|
| Charger Type              | %u           | Connected charger type, more in Battery.hpp "BatteryChargeInfoFieldsChargerType" |
| Charger Max Voltage       | %u           | Charger and external device voltage limit in mV                                  |
| Charger Max Current       | %u           | Charger and external device current limit in mA                                  |

> Miscellaneous

| Category               | Format            | Explanation                                                                      |
|------------------------|-------------------|----------------------------------------------------------------------------------|
| DSP Usage              | %u                | In percent (not available on 17.0.0+)                                            |
| NVDEC                  | %.2f              | Target frequency in MHz (shows 0 if chip is not active)                          |
| NVENC                  | %.2f              | Target frequency in MHz (shows 0 if chip is not active)                          |
| NVJPG                  | %.2f              | Target frequency in MHz (shows 0 if chip is not active)                          |
| Network Type           | %s                | It shows if Switch is connected to internet, and if it's using Ethernet or Wi-Fi |

If Network Type is "Wi-Fi", you can press Y to show password. Since max password length is 64 characters, it may show in up to 3 lines.

> Game Resolutions

For this mode to show and work properly you must have SaltyNX 0.9.0+ installed.

When game runs, this menu shows what resolutions and how many times they were passed to GPU via two functions:
- __Depth__ shows info from depth texture passed to `nvnCommandBufferSetRenderTargets`
- __Viewport__ shows info from arguments passed to `nvnCommandBufferSetViewport` and `nvnCommandBufferSetViewports`

This menu shows first 8 resolutions passed to those functions in last frame rendering loop, sorted in descending order of calls number.<br>
Its main purpose is to catch game rendering resolution, but user must deduce which ones are correct.<br>
I have limited catched resolutions only to ones that have ratio higher than 1.70 and lower than 1.90.<br>

Remember that resolutions you can see in this mode may be used in different ways - for example Tokyo Xanadu Ex+ max dynamic resolution in handheld will show 1280x736, but it's not that game will squeeze this into 720p screen, it's just removing those additional 16 pixels from showing on screen.

Those commands are used by all 3D games using NVN API (that's why it won't work with other APIs and may not work with games using 2D engines).<br>
This mode is not 100% fullproof, so it can show that nothing is catched or it won't catch what is used for 3D rendering (if this happens for some 3D game, please report an issue).

By default refresh rate of this menu is 10 FPS. You can change that in config.ini, more in config.md<br>
Exiting is done by using the same combo buttons used in other main modes.

Games that don't mesh well with RES category in `Mini` and Resolution in `Full` modes while showing properly rendering resolutions in this menu:
- `Kirby and The Forgotten Land`: dynamic resolution has much less calls than other resolutions, it shows up correctly only when it's max dynamic resolution set. It shows up always first in Depth category of Game Resolutions menu.
- `Darksiders 2`: Dynamic resolution is randomly showing up, fighting for attention with native resolution call. In Game Resolutions menu it shows up either at 2nd or 3rd place of Viewport.

# Additional info

> How Battery Remaining Time is calculated

It is calculated from the last 3 minutes of power draw readings. In the first minute of running any mode, it is updated every 0.5s, after one minute it's updated every minute. When exiting the currently used mode, calculations are reset. When connecting or disconnecting the charger, calculations are reset. When Battery Power Draw is positive instead of negative, time is shown as [-:--] and calculations are reset.

> What is Skin temperature ($\mathbf{T}skin$)?

This is temperature calculated from SoC and PCB temperatures that is mainly used to prevent Switch from overheating. Reading it is not supported on FWs <5.0.0.

Explanation provided by CTCaer
>Temperature ($\mathbf{T}skin$) calculation.
>
>
>Console and Handheld:
>
>$\mathbf{T}soc >= 84°C$ - Immediate sleep. No questions asked.
>
>$\mathbf{T}pcb >= 84°C$ - Immediate sleep. No questions asked.
>
>
>Console only:
>
>$\mathbf{T}skin < 63°C$ - Clears both timers.
>
>$\mathbf{T}skin >= 63°C$ - Immediate sleep. No questions asked.
>
>
>Handheld only:
>
>$\mathbf{T}skin < 58°C$ - Clears both timers.
>
>$58°C <= \mathbf{T}skin < 61°C$ - Starts 60s timer and clears 10s timer.
>
>$61°C <= \mathbf{T}skin < 63°C$ - Starts 10s timer.
>
>$\mathbf{T}skin >= 63°C$ - Immediate sleep. No questions asked.
>
>
>The two timers are separate. If 61°C is reached, the 10s timer is immediatly started.
>
>If a timer ends and the temperature is not dropped, goes into sleep.
>
>
>tskin is the specific calculation below:
```
soc_adj = (temps.iir_filter_gain_soc * (temps.soc - temps.soc_adj_prev)) + temps.soc_adj_prev;
pcb_adj = (temps.iir_filter_gain_pcb * (temps.pcb - temps.pcb_adj_prev)) + temps.pcb_adj_prev;
temps.soc_adj_prev = soc_adj;
temps.pcb_adj_prev = pcb_adj;

if (soc_adj >= 38000)
{
    if (temps.handheld)
        soc_adj = (temps.tskin_soc_coeff_handheld[0] * soc_adj) + (1000 * temps.tskin_soc_coeff_handheld[1]);
    else
        soc_adj = (temps.tskin_soc_coeff_console[0] * soc_adj) + (1000 * temps.tskin_soc_coeff_console[1]);
    soc_adj = (soc_adj / 10000) + 500;
}

if (pcb_adj >= 38000)
{
    if (temps.handheld)
        pcb_adj = (temps.tskin_pcb_coeff_handheld[0] * pcb_adj) + (1000 * temps.tskin_pcb_coeff_handheld[1]);
    else
        pcb_adj = (temps.tskin_pcb_coeff_console[0] * soc_adj) + (1000 * temps.tskin_pcb_coeff_console[1]);
    pcb_adj = (pcb_adj / 10000) + 500;
}

skin_adj = MAX(soc_adj, pcb_adj); //tskin
```
