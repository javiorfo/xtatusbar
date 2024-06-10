#include "xtatusbar.h"

static Component components[] = {
    { "  CPU %hd%% | ", 1000, get_cpu_usage },
    { "  RAM %hd%% | ", 1000, get_ram_usage },
    { "󰏈  TEMP %hd°C ", 1000, get_cpu_temperature },
};

// static const char *TEMPERATURE_FILE = "/sys/class/thermal/thermal_zone0/temp";
static const char* TEMPERATURE_FILE = "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input";
// static const char *DATE_FORMAT = "";


