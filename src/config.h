#include "xtatusbar.h"

static Component components[] = {
    { "󰏈  TEMP %s°C ", 1000, get_cpu_temperature },
    { "  CPU %s%% ", 500, get_cpu_usage },
};
