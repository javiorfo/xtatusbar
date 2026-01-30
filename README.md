# xtatusbar
*Configurable statusbar for Xorg server using xsetroot*

## Caveats
- Dependencies: `Xlib` and `asoundlib`
- Influenced by suckless software (but recompilation not needed).
- This library has been developed on and for Linux following open source philosophy.

## Installation
- Using make:
```bash
git clone https://github.com/javiorfo/xtatusbar
cd xtatusbar
sudo make clean install
```

- Using yay (AUR Arch Linux):
```bash
yay -S xtatusbar
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
| WEATHER | :heavy_check_mark: | using [wttr](https://wttr.in) and [SCRIPT COMPONENT](https://github.com/javiorfo/xtatusbar/tree/master/scripts/weather.sh) |
| NETWORK STATUS | :heavy_check_mark: | Receive and send packages |
| DATE | :heavy_check_mark: | could be custimizable |
| SCRIPT COMPONENT | :heavy_check_mark: | Shell script |

## Customizable by parameters
- By default the values by parameters are the following:
```c
// First parameter is a boolean to enable or disable the component
// Second parameter is the time in miliseconds in which every component will get their values
// Third parameter is a string interpolation
// Fifth parameter is the optional configuration file
Group cpu = {true, 2000, "  CPU %d%%", Cpu};
Group ram = {true, 2000, "  RAM %d%%", Ram};
Group temp = {true, 2000, "󰏈  TEMP %d°C", Temperature,
              "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input"};
Group disk = {true, 10000, "󰋊  DISK %d%%", Disk};
Group volume = {true, 200, "  VOL %s", Volume};
Group network = {true, 5000, "%s NET", Network};
Group battery = {false, 5000, "  BAT %d%%", Battery,
                     "/sys/class/power_supply/BAT0/capacity"};
Group script = {false, 2000, "CUSTOM %s", Script};
Group date = {true, 60000, "  %A %d/%m/%Y %H:%M", Date};
// Separator between components
char *separator = "|";
```

- Option parameters
```text
Usage:
  xtatusbar [options]

Each component (bat, script, cpu, ram, temp, disk, vol, net, date) supports the following flags:

Available flags:
  --separator               String separator between components
  --{component}-enabled     Boolean (true or false)
  --{component}-time        Time in miliseconds in which the component will refresh
  --{component}-pattern     String pattern to show
  --{bat|temp|script}-file  File path. Only for battery, temperature and script

```

## Screenshots

<img src="https://github.com/javiorfo/img/blob/master/xtatusbar/xtatusbar.png?raw=true" alt="xtatusbar" />

---

### Donate
- **Bitcoin** [(QR)](https://raw.githubusercontent.com/javiorfo/img/master/crypto/bitcoin.png)  `1GqdJ63RDPE4eJKujHi166FAyigvHu5R7v`
- [Paypal](https://www.paypal.com/donate/?hosted_button_id=FA7SGLSCT2H8G)
