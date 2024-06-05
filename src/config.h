#include "xtatusbar.h"

static Component components[] = {
    { "TEMP %s°C ", 500000,  get_cpu_temperature, ""  },
    { "CPU %s%% ",  500000,  get_cpu_usage,       ""  },
};
