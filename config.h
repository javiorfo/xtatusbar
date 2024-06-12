#include "xtatusbar.h"

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
};

// File where to get PC temperature
// static const char *TEMPERATURE_FILE = "/sys/class/thermal/thermal_zone0/temp";
static const char* TEMPERATURE_FILE = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";

// Date format calendar
static const char* DATE_FORMAT = "%A %d/%m/%Y %H:%M";

// Path to a script if execute_script function is used
static const char* SCRIPT = "./path/to/script";
