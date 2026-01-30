#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct Component;
typedef void (*function)(struct Component*);

typedef struct Component {
    char pattern[30];
    uint64_t wait_ms;
    function fn;
    char *result;
    struct timeval last_time;
    char *file;
    size_t pattern_len;
} Component;

void get_temperature(Component*);
void get_cpu(Component*);
void get_ram(Component*);
void get_disk(Component*);
void get_date(Component*);
void get_network(Component*);
void get_volume(Component*);
void get_battery(Component*);
void execute(Component*);
