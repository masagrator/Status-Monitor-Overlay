# Modes

Status Monitor Overlay from 0.8 release contains six modes to choose from Main Menu.<br>
For additional functions you need to install:
- [SaltyNX](https://github.com/masagrator/SaltyNX/releases)
- [sys-clk 2.0.0+](https://github.com/retronx-team/sys-clk/releases) (using closed source forks of sys-clk can result in retrieving wrong real clockrates)

# Full

This mode you can know from older releases of Status Monitor. It contains all informations properly described and supported with high precision.

| Category  | Format                                                                              | Explanation                                                                                                                                                                                                                                                                                                               |
|-----------|-------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| CPU Usage | Frequency: %.1f (Δ%+.1f)<br>Core #0: %.2f<br>Core #1: %.2f<br>Core #2: %.2f<br>Core #3: %.2f | Targete clockrate of all CPU cores in MHz (^1)<br>Load of CPU Core #0 calculated from IdleTickCount to percent value<br>Load of CPU Core #1 calculated from IdleTickCount to percent value<br>Load of CPU Core #2 calculated from IdleTickCount to percent value<br>Load of CPU Core #3 calculated from IdleTickCount to percent value |
| GPU Usage | Frequency: %.1f (Δ%+.1f)<br>Load: %.1f                                                       | Target clockrate of GPU in MHz (^1)<br>GPU Load provided by PMU in percent                                                                                                                                                                                                                                                            |
| RAM Usage | Frequency: %.1f (Δ%+.1f)<br>%s: %.2f/%.2f                                                    | Target clockrate of EMC in MHz (^1)<br>%s memory used/%s memory available in MB (not working with FW <5.0.0)                                                                                                                                                                                                                          |
| Board | Battery Power Flow: %+.2f[h:mm]<br>Temperatures:<br>- SoC: %.1f <br>- PCB: %.1f <br>- Skin: %.1f<br>Fan Rotation Level: %.1f                                   | How much power in watts is discharged from or charged to the battery [time left before shutdown]<br>SoC temperature in Celsius degrees<br>PCB temperature in Celsius degrees<br>Skin temperature in Celsius degrees (^2)<br>Fan rotation level in percent                                                                                                                                                                         |

- ^1 - Difference between real clocks and targeted clocks. This shows only when sys-clk 2.0.0+ is installed.
- ^2 - Explanation provided at the end of file

```Optional (shows only when SaltyNX is installed)```

| Category | Format            | Explanation                                                              |
|----------|-------------------|--------------------------------------------------------------------------|
| PFPS     | %u                | Pushed Frames Per Second - how many frames were displayed in last second |
| FPS      | %.2f              | Frames Per Second - value calculated by averaging frametime              |

Works only in 1 and 5 Hz + vsync signal. You can change that with ZR + R + D-Pad Up/Down

# Mini

Contains most of supported informations with lower precision.

| Category | Format                                           | Explanation                                                               |
|----------|--------------------------------------------------|---------------------------------------------------------------------------|
| CPU      | [%.0f,%.0f,%.0f,%.0f]@%.1f                       | Core #0 usage, Core #1 usage, Core #2 usage, Core #3 usage@CPU target frequency  |
| GPU      | %.1f@%.1f                                        | Load@GPU Target Frequency                                                 |
| RAM      | %.0f/%.0f@%.1f                                   | Total RAM used/Total RAM available in MB@EMC Target frequency             |
| TEMP     | %2.1f/%2.1f/%2.1f                                | SoC temperature/PCB temperature/Skin temperature (^1)                     |
| FAN      | %2.1f                                            | Fan rotation level                                                        |
| DRAW     | %+.2f[h:mm]                                      | How much power in watts is discharged from or charged to the battery [Time left before shutdown]      |

- ^1 - Explanation provided at the end of file

```Optional```

> shows only when SaltyNX is installed and game is running
> 
| Category | Format            | Explanation                                                              |
|----------|-------------------|--------------------------------------------------------------------------|
| FPS      | %.2f              | Frames Per Second                                                        |

> shows only when sys-clk 2.0.0+ is installed
> 
| Category | Format              | Explanation                                                              |
|----------|---------------------|--------------------------------------------------------------------------|
| DIFF     | %+2.1f %2.1f %+2.1f | CPU / GPU / RAM difference between real clocks and targeted clocks       |

Works only in 1 or 5 Hz + vsync signal. You can change that with ZR + R + D-Pad Up/Down

# Micro

Contains most of supported informations with lower precision in one line.

| Category | Format                                                     | Explanation                                                               |
|----------|------------------------------------------------------------|---------------------------------------------------------------------------|
| CPU      | [%.0f,%.0f,%.0f,%.0f]%s%.1f                                 | [Core #0 usage, Core #1 usage, Core #2 usage, Core #3 usage] (^1) CPU Target Frequency  |
| GPU      | %.1f%s%.1f                                                  | Load (^1) GPU Target Frequency                                                        |
| RAM      | %.1f/%.1f%s%.1f                                             | Total RAM used/Total RAM available in GB (^1) EMC Target frequency                    |
| BRD      | %2.1f/%2.1f/%2.1f@+.1f[h:mm]                                | SoC temperature/PCB temperature/Skin temperature(^2)/Battery Power Flow[Time left before shutdown]      |
| FAN      | %2.2f                                                       | Fan rotation level                                                               |

- ^1 - by default it's `@`, but if you have sys-clk 2.0.0+ installed, this changes depending on difference between real clocks and targeted clocks. <br>
  - `△` - real clocks are higher by at least 10 MHz than expected
  - `▽` - real clocks are lower  by at least 10 MHz than expected
  - `≠` - real clocks are lower by at least 50 MHz than expected (If this shows constantly for longer than few seconds, this is a sign of throttling, usually caused by bad overclocking configuration)
- ^2 - Explanation provided at the end of file

```Optional```

> shows only when SaltyNX is installed and game is running

| Category | Format             | Explanation                                                              |
|----------|--------------------|--------------------------------------------------------------------------|
| FPS      | %.1f               | Frames Per Second                                                        |

Works only in 1 or 5 Hz + vsync signal. You can change that with ZR + R + D-Pad Up/Down

# FPS Counter

It shows only FPS value in 31Hz + vsync signal. <br>
If game is not launched, it will show always 254.0 value.<br>
You can move counter to the bottom or top of the screen by pressing ZR + R + D-Pad Up/Down.

Mode available only with SaltyNX installed.

# Graphs

> FPS

It shows average FPS graph in 31Hz + vsync signal. In background of graph you can see rendered actual average FPS.<br>
If game is not launched, it will show always 254.0 value and graph will be empty.<br>
If game is not rendering any new frame, graph is not updated.

If line is within rounding error of graph line, it's colored. Average FPS must be between -0.05 and +0.04 of graph line target FPS. So for 30 FPS it's between 29.95 - 30.04 FPS. For 43 FPS it's 42.95 - 43.04 FPS.
If it's in 30 or 60 FPS rounding error range, it's green. Any other FPS graph target is purple.

You can move graph to the bottom or top of the screen by pressing ZR + R + D-Pad Up/Down.

Mode available only with SaltyNX installed.

# Other

> Battery

| Category                        | Format    | Explanation                                                                                   |
|---------------------------------|-----------|-----------------------------------------------------------------------------------------------|
| Battery Temperature             | %.2f      | Battery temperature in Celsius                                                                |
| Battery Raw Charge              | %.2f      | Raw battery charged capacity in percent                                                       |
| Battery Voltage (AVG of 5)      | %.2f      | Battery average voltage in mV taken from 5 readings in period of 5 seconds                    |
| Battery Current Flow (AVG of 5) | %+.2f     | Battery average current flow in mA taken from 5 readings in period of 5 seconds               |
| Battery Power Flow (AVG of 5)   | %+.3f     | Battery average power flow in W calciulated from Battery Voltage and Battery Current Flow     |
| Battery Remaining Time          | h:mm      | How much time is left before shutdown                                                         |

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
