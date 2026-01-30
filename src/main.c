#include "component.h"
#include "util.h"
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STATUSBAR_MAX_STRING_LENGTH 200

int main(int argc, char *argv[]) {
    static Component components[9];
    int size = process(argc, argv, components);

    Display *display = XOpenDisplay(NULL);
    if (!display) {
        perror("Cannot open display\n");
        return EXIT_FAILURE;
    }

    Window root = DefaultRootWindow(display);

    while (true) {
        char final_str[STATUSBAR_MAX_STRING_LENGTH];
        int current_len = 0;

        for (int i = 0; i < size; i++) {
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
        msleep(200);
    }

    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
