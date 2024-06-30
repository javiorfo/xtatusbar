# xtatusbar
*Configurable statusbar for Xorg server using xsetroot*

## Caveats
- Dependencies: `xsetroot`
- Influenced by suckless software (changes are doing by config.h file and recompilation).
- This library has been developed on and for Linux following open source philosophy.

## Installation
- First step:
```bash
git clone https://github.com/chaosystema/xtatusbar
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
| NETWORK STATUS | :heavy_check_mark: | Up or down |
| DATE | :heavy_check_mark: | could be custimizable |
| CUSTOM COMPONENT | :heavy_check_mark: | coding in C |

## Customizable config.h
- Edit [config.h](https://github.com/chaosystema/xtatusbar/blob/master/config.h) to change values or configurations (then recompile)
- By default the values by parameters are the following:
```c
// First element is a string interpolation
// Second element is the time in miliseconds in which every component will execute the function in the third element
// Third element is the function to be executed
static Component components[] = {
    { "  CPU %hd%% | ", 1000, get_cpu_usage },
    { "  RAM %hd%% | ", 1000, get_ram_usage },
    { "󰏈  TEMP %hd°C | ", 1000, get_cpu_temperature },
    { "󰋊  DISK %hd%% | ", 5000, get_disk_usage },
    { "󰋊  VOL %s | ", 100, get_volume },
    { "%s NET | ", 5000, network_is_connected },
    { "  %s ", 60000, get_date },
    // { "  BAT %hd%% | ", 5000, get_battery_status },
    // { "CUSTOM %s ", 1000, execute_script },
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

<img src="https://github.com/chaosystema/img/blob/master/xtatusbar/xtatusbar.png?raw=true" alt="xtatusbar" />

---

### Donate
- **Bitcoin** [(QR)](https://raw.githubusercontent.com/chaosystema/img/master/crypto/bitcoin.png)  `1GqdJ63RDPE4eJKujHi166FAyigvHu5R7v`
