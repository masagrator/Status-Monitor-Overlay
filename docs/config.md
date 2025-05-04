# Configuration file

Configuration file is in `sdmc:/config/status-monitor/`. By default it's named `config.ini.template`. To be detected by overlay it must be renamed to `config.ini`.

---

Explanation of what each setting does.<br>
Colors are provided in RGBA4444 format, which means that each character represents different channel: red, green, blue, alpha.

> [status-monitor]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `key_combo` | Buttons combination that allows exiting Full, Mini, Micro, FPS Graph and FPS Counter modes; max 4 buttons, otherwise next buttons will be ignored. Combine different buttons with `+` | `A`, `B`, `X`, `Y`, `L`, `R`, `ZL`, `ZR`, `PLUS`, `MINUS`, `DUP`, `DDOWN`, `DLEFT`, `DRIGHT`, `SL`, `SR`, `LSTICK`, `RSTICK`, `UP`, `DOWN`, `LEFT`, `RIGHT` | `L+DDOWN+RSTICK` |
| `battery_avg_iir_filter` | Read voltage + current averages directly from fuel gauge, that uses infinite impulse response filter | `true`, `false` | `false` |
| `battery_time_left_refreshrate` | How many seconds must pass to refresh Battery Remaining Time | from `1` to `60` | `60` |
| `average_gpu_load` | Average 5 last GPU load readings. Because GPU load by design shows only load for last 1/60s, this option allows for more accurate gpu load readings at the cost of slight bump in CPU Core #3 usage (from 2 to 4 percentage points depending on CPU clock). | `true`, `false` | `false` |
| `font_cache` | Allow caching generated glyphs for better performance. Cache is cleaned up when moving between modes. | `true`, `false` | `true` |


> [full]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `refresh_rate` | How often per second this mode should be refreshed. Higher value means higher CPU Core #3 usage, that's why it is recommended to stay at 1. | From `1` to `60` | `1` |
| `layer_width_align` | On which side of the screen X axis you want this mode | `left`, `right` | `left` |
| `show_real_freqs` | This works only if you have sys-clk 2.0.0_rc4+ installed. Show real frequencies for CPU, GPU and RAM | `false`, `true` | `true` |
| `show_deltas` | This works only if you have sys-clk 2.0.0_rc4+ installed. Show difference for CPU, GPU and RAM between target frequencies and real frequencies. They are not showed if both keys `show_real_freqs` and `show_target_freqs` are set to `false` | `false`, `true` | `true` |
| `show_target_freqs` | Show target frequencies for CPU, GPU and RAM | `false`, `true` | `true` |
| `show_fps` | Show game's FPS and PFPS | `false`, `true` | `true` |
| `show_res` | Show game's 2 best candidates for internal resolution, it won't show when game is not using NVN | `false`, `true` | `true` |

> [mini]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `refresh_rate` | How often per second this mode should be refreshed. Higher value means higher CPU Core #3 usage, that's why it is recommended to stay at 1. | From `1` to `60` | `1` |
| `layer_width_align` | On which side of the screen X axis you want this mode | `left`, `center`, `right` | `left` |
| `layer_height_align` | On which side of the screen Y axis you want this mode | `top`, `center`, `bottom` | `top` |
| `handheld_font_size` | How big should be rendered text in handheld mode | From `8` to `22` | `15` |
| `docked_font_size` | How big should be rendered text in docked mode | From `8` to `22` | `15` |
| `real_freqs` | This works only if you have sys-clk 2.0.0_rc4+ installed. It shows real frequencies for CPU, GPU and RAM instead of target frequencies | `true`, `false` | `false` |
| `show` | Which data should be rendered. Connect different stats with `+` in any order. | `CPU`, `GPU`, `RAM`, `TEMP`, `FAN`, `DRAW`, `FPS`, `RES` | `CPU+GPU+RAM+TEMP+FAN+DRAW+FPS+RES` |
| `replace_MB_with_RAM_load` | This works only if you have sys-clk 2.0.0_rc4+ installed. Instead of RAM total usage it shows RAM load. | `true`, `false` | `true` |
| `background_color` | Background color in RGBA4444 format | From `#0000` to `#FFFF` | `#1117` |
| `cat_color` | Category text color (left side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
| `text_color` | Stats text color (right side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |

> [micro]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `refresh_rate` | How often per second this mode should be refreshed. Higher value means higher CPU Core #3 usage, that's why it is recommended to stay at 1. | From `1` to `60` | `1` |
| `layer_height_align` | On which side of the screen Y axis you want this mode | `top`, `bottom` | `top` |
| `handheld_font_size` | How big should be rendered text in handheld mode | From `8` to `18` | `18` |
| `docked_font_size` | How big should be rendered text in docked mode | From `8` to `18` | `18` |
| `text_align` | How shown text should be aligned | `left`, `center`, `right`| `center` |
| `real_freqs` | This works only if you have sys-clk 2.0.0_rc4+ installed. It shows real frequencies for CPU, GPU and RAM instead of target frequencies | `true`, `false` | `false` |
| `show` | Which data should be rendered. Connect different stats with `+` in any order. | `CPU`, `GPU`, `RAM`, `BRD`, `FAN`, `FPS` | `CPU+GPU+RAM+BRD+FAN+FPS` |
| `replace_GB_with_RAM_load` | This works only if you have sys-clk 2.0.0_rc4+ installed. Instead of RAM total usage it shows RAM load. | `true`, `false` | `true` |
| `background_color` | Background color in RGBA4444 format | From `#0000` to `#FFFF` | `#1117` |
| `cat_color` | Category text color (left side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FCCF` |
| `text_color` | Stats text color (right side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |


> [fps-counter]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `layer_width_align` | On which side of the screen X axis you want this mode | `left`, `center`, `right` | `left` |
| `layer_height_align` | On which side of the screen Y axis you want this mode | `top`, `center`, `bottom` | `top` |
| `handheld_font_size` | How big should be rendered text in handheld mode | From `8` to `150` | `40` |
| `docked_font_size` | How big should be rendered text in docked mode | From `8` to `150` | `40` |
| `background_color` | Background color in RGBA4444 format | From `#0000` to `#FFFF` | `#1117` |
| `text_color` | Text color in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |

> [fps-graph]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `layer_width_align` | On which side of the screen X axis you want this mode | `left`, `center`, `right` | `left` |
| `layer_height_align` | On which side of the screen Y axis you want this mode | `top`, `center`, `bottom` | `top` |
| `max_fps_text_color` | Text color of "60" in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
| `min_fps_text_color` | Text color of "0" in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
| `background_color` | Background color in RGBA4444 format | From `#0000` to `#FFFF` | `#1117` |
| `fps_counter_color` | Text color of average FPS printed behind graph in RGBA4444 format | From `#0000` to `#FFFF` | `#4444` |
| `border_color` | Color of rectangle in RGBA4444 format | From `#0000` to `#FFFF` | `#F77F` |
| `dashed_line_color` | Color of dashed line at the center of rectangle in RGBA4444 format | From `#0000` to `#FFFF` | `#8888` |
| `main_line_color` | Color of line representing FPS value on graph in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
| `rounded_line_color` | Color of line representing FPS value on graph if it's divisble by 10 in RGBA4444 format | From `#0000` to `#FFFF` | `#F0FF` |
| `perfect_line_color` | Color of line representing FPS value on graph if it's divisble by 30 in RGBA4444 format | From `#0000` to `#FFFF` | `#0C0F` |

> [game_resolutions]

| Key | Explanation | Possible values | Default Value |
|-----|-------------|-----------------|---------------|
| `refresh_rate` | How often per second this mode should be refreshed. Higher value means higher CPU Core #3 usage, that's why it is recommended to stay at 1. | From `1` to `60` | `10` |
| `layer_width_align` | On which side of the screen X axis you want this mode | `left`, `center`, `right` | `left` |
| `layer_height_align` | On which side of the screen Y axis you want this mode | `top`, `center`, `bottom` | `top` |
| `background_color` | Background color in RGBA4444 format | From `#0000` to `#FFFF` | `#1117` |
| `cat_color` | Category text color (left side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
| `text_color` | Stats text color (right side) in RGBA4444 format | From `#0000` to `#FFFF` | `#FFFF` |
