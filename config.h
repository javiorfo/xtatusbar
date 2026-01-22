#include "xtatusbar.h"
#include <stdint.h>

// First element is a string interpolation
// Second element is the time in miliseconds in which every component will execute the function in the third element
// Third element is the function to be executed
static Component components[] = {
    {.pattern = "  CPU %d%% | ", .wait_ms = 2000, .fn = cpu},
    {.pattern = "  RAM %d%% | ", .wait_ms = 2000, .fn = ram},
    {.pattern = "󰏈  TEMP %d°C | ", .wait_ms = 2000, .fn = temperature},
    {.pattern = "󰋊  DISK %d%% | ", .wait_ms = 10000, .fn = disk},
    {.pattern = "  VOL %s | ", .wait_ms = 200, .fn = volume},
    {.pattern = "%s NET | ", .wait_ms = 5000, .fn = network},
    {.pattern = "  %s ", .wait_ms = 60000, .fn = date},
/*     {.pattern = "  BAT %d%% | ", .wait_ms = 5000, .fn = battery},
    {.pattern = "CUSTOM %s ", .wait_ms = 1000, .fn = script}, */
};

// File where to get PC temperature
// static const char *TEMPERATURE_FILE = "/sys/class/thermal/thermal_zone0/temp";
static const char* TEMPERATURE_FILE = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";

// Date format calendar
static const char* DATE_FORMAT = "%A %d/%m/%Y %H:%M";

// File where to get battery info
static const char* BATTERY_FILE = "/sys/class/power_supply/BAT0/capacity";

// Path to a script if execute_script function is used
static const char* SCRIPT = "./path/to/script";
