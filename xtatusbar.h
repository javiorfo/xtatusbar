#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

struct Component;
typedef void (*function)(struct Component *c);

typedef struct Component {
    char *pattern;
    uint64_t wait_ms;
    function fn;
    char *result;
    struct timeval last_time;
} Component;

void temperature(Component*);
void cpu(Component*);
void ram(Component*);
void disk(Component*);
void date(Component*);
void network(Component*);
void volume(Component*);
void battery(Component*);
void execute(Component*);
bool must_generate(Component *c);
