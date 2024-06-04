#include "xtatusbar.h"

static Component components[] = {
    { "Uno %d ", 1000000, test, "1" },
    { "TEMP %d°C ", 500000, get_cpu_temperature, "" },
    { "CPU %d%% ", 500000, get_cpu_usage, "" },
};
