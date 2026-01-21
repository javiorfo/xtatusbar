# xtatusbar
*Configurable statusbar for Xorg server using xsetroot*

## Caveats
- Dependencies: `xsetroot` and `asoundlib`
- Influenced by suckless software (changes are doing by config.h file and recompilation).
- This library has been developed on and for Linux following open source philosophy.

## Installation
- First step:
```bash
git clone https://github.com/javiorfo/xtatusbar
cd xtatusbar
sudo make clean install
```

- In your **~/.xinitrc** to start in every login
```bash
xtatusbar &
```

## Overview
| Component | xtatusbar | NOTE |
| ------- | ------------- | ---- |
| CPU usage | :heavy_check_mark: | percentage |
| RAM usage | :heavy_check_mark: | percentage |
| TEMPERATURE | :heavy_check_mark: | Celcious |
| DISK USAGE | :heavy_check_mark: | percentage |
| VOLUME LEVEL | :heavy_check_mark: | Level and Mute status |
| BATTERY LEVEL | :heavy_check_mark: | percentage |
| WEATHER | :x: | |
| NETWORK STATUS | :heavy_check_mark: | Receive and send packages |
| DATE | :heavy_check_mark: | could be custimizable |
| CUSTOM COMPONENT | :heavy_check_mark: | coding in C |

## Customizable config.h
- Edit [config.h](https://github.com/javiorfo/xtatusbar/blob/master/config.h) to change values or configurations (then recompile)
- By default the values by parameters are the following:
```c
// First element is a string interpolation
// Second element is the time in miliseconds in which every component will execute the function in the third element
// Third element is the function to be executed
static Component components[] = {
    {.pattern = "  CPU %d%% | ", .wait_ms = 1000, .fn = cpu},
    {.pattern = "  RAM %d%% | ", .wait_ms = 1000, .fn = ram},
    {.pattern = "󰏈  TEMP %d°C | ", .wait_ms = 1000, .fn = temperature},
    {.pattern = "󰋊  DISK %d%% | ", .wait_ms = 5000, .fn = disk},
    {.pattern = "  VOL %s | ", .wait_ms = 100, .fn = volume},
    {.pattern = "%s NET | ", .wait_ms = 5000, .fn = network},
    {.pattern = "  %s ", .wait_ms = 1000, .fn = date},
/*     {.pattern = "  BAT %d%% | ", .wait_ms = 5000, .fn = battery},
    {.pattern = "CUSTOM %s ", .wait_ms = 1000, .fn = script}, */
};

// File where to get PC temperature
// static const char *TEMPERATURE_FILE = "/sys/class/thermal/thermal_zone0/temp";
static const char* TEMPERATURE_FILE = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";

// File where to get battery info
static const char* BATTERY_FILE = "/sys/class/power_supply/BAT0/uevent";

// Date format calendar
static const char* DATE_FORMAT = "%A %d/%m/%Y %H:%M";

// Path to a script if execute_script function is used
static const char* SCRIPT = "./path/to/script";

```

## Screenshots

<img src="https://github.com/javiorfo/img/blob/master/xtatusbar/xtatusbar.png?raw=true" alt="xtatusbar" />

---

### Donate
- **Bitcoin** [(QR)](https://raw.githubusercontent.com/javiorfo/img/master/crypto/bitcoin.png)  `1GqdJ63RDPE4eJKujHi166FAyigvHu5R7v`
- [Paypal](https://www.paypal.com/donate/?hosted_button_id=FA7SGLSCT2H8G)
