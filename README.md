# xtatusbar
*Configurable statusbar for Xorg server using xsetroot*

## Caveats
- Dependencies: `xsetroot`
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
| BATTERY LEVEL | :x: |  |
| WEATHER | :x: | |
| NETWORK STATUS | :heavy_check_mark: | Up or down |
| DATE | :heavy_check_mark: | could be custimizable |
| CUSTOM COMPONENT | :heavy_check_mark: | coding in C |

## Customizable config.h
- By default the values by parameters are:
```c
// First element is a string interpolation
// Second element is the time in miliseconds in which every component will execute the function in the third element
// Third element is the function to be executed
static Component components[] = {
    { "  CPU %hd%% | ", 2000, get_cpu_usage },
    { "  RAM %hd%% | ", 2000, get_ram_usage },
    { "󰏈  TEMP %hd°C | ", 2000, get_cpu_temperature },
    { "󰋊  DISK %hd%% | ", 5000, get_disk_usage },
    { "󰋊  VOL %s | ", 100, get_volume },
    { "%s NET | ", 5000, network_is_connected },
    { "  %s ", 60000, get_date },
};

static const char* TEMPERATURE_FILE = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";
static const char *DATE_FORMAT = "%A %d/%m/%Y %H:%M";
```

## Screenshots

<img src="https://github.com/javiorfo/img/blob/master/xtatusbar/xtatusbar.png?raw=true" alt="xtatusbar" />

---

### Donate
- **Bitcoin** [(QR)](https://raw.githubusercontent.com/javiorfo/img/master/crypto/bitcoin.png)  `1GqdJ63RDPE4eJKujHi166FAyigvHu5R7v`
- [Paypal](https://www.paypal.com/donate/?hosted_button_id=FA7SGLSCT2H8G)
