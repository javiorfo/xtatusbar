#include "config.h"
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STATUSBAR_MAX_STRING_LENGTH 200
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]));

static const size_t COMP_COUNT = ARRAY_LENGTH(components);

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        perror("Cannot open display\n");
        exit(1);
    }

    Window root = DefaultRootWindow(display);

    while (true) {
        char final_str[STATUSBAR_MAX_STRING_LENGTH];
        int current_len = 0;

        for (int i = 0; i < COMP_COUNT; i++) {
            Component *c = components + i;
            c->fn(c);

            int written =
                snprintf(final_str + current_len,
                         sizeof(final_str) - current_len, "%s", c->result);

            if (written >= (int)(sizeof(final_str) - current_len)) {
                current_len = sizeof(final_str) - 1;
                break;
            }

            current_len += written;
        }

        XStoreName(display, root, final_str);

        XFlush(display);

        usleep(MILISECONDS_TO_MICROSECONDS(200));
    }

    XCloseDisplay(display);

    return 0;
}
