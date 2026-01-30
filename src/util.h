#include "component.h"
#include <stdint.h>

enum Type {
    Cpu,
    Ram,
    Temperature,
    Disk,
    Volume,
    Network,
    Date,
    Battery,
    Script,
};

typedef struct {
    bool enabled;
    uint64_t time;
    char *pattern;
    enum Type type;
    char *file;
} Group;

int process(int, char *[], Component[]);
bool must_generate(Component*);
void msleep(long msec);
