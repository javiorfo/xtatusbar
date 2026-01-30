#include "util.h"
#include "component.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

int process(int argc, char *argv[], Component components[]) {
    Group battery = {false, 5000, "  BAT %d%%", Battery,
                     "/sys/class/power_supply/BAT0/capacity"};
    Group script = {false, 2000, "CUSTOM %s", Script};
    Group cpu = {true, 2000, "  CPU %d%%", Cpu};
    Group ram = {true, 2000, "  RAM %d%%", Ram};
    Group temp = {true, 2000, "󰏈  TEMP %d°C", Temperature,
                  "/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input"};
    Group disk = {true, 10000, "󰋊  DISK %d%%", Disk};
    Group volume = {true, 200, "  VOL %s", Volume};
    Group network = {true, 5000, "%s NET", Network};
    Group date = {true, 60000, "  %A %d/%m/%Y %H:%M", Date};
    char *separator = "|";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--separator") == 0) separator = argv[++i];

        // --- BATTERY ---
        else if (strcmp(argv[i], "--bat-enabled") == 0 && i + 1 < argc)
            battery.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--bat-time") == 0 && i + 1 < argc)
            battery.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--bat-pattern") == 0 && i + 1 < argc)
            battery.pattern = argv[++i];
        else if (strcmp(argv[i], "--bat-file") == 0 && i + 1 < argc)
            battery.file = argv[++i];

        // --- SCRIPT ---
        else if (strcmp(argv[i], "--script-enabled") == 0)
            script.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--script-time") == 0 && i + 1 < argc)
            script.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--script-pattern") == 0 && i + 1 < argc)
            script.pattern = argv[++i];
        else if (strcmp(argv[i], "--script-file") == 0 && i + 1 < argc)
            script.file = argv[++i];

        // --- CPU ---
        else if (strcmp(argv[i], "--cpu-enabled") == 0)
            cpu.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--cpu-time") == 0 && i + 1 < argc)
            cpu.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--cpu-pattern") == 0 && i + 1 < argc)
            cpu.pattern = argv[++i];

        // --- RAM ---
        else if (strcmp(argv[i], "--ram-enabled") == 0)
            ram.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--ram-time") == 0 && i + 1 < argc)
            ram.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--ram-pattern") == 0 && i + 1 < argc)
            ram.pattern = argv[++i];

        // --- TEMP ---
        else if (strcmp(argv[i], "--temp-enabled") == 0)
            temp.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--temp-time") == 0 && i + 1 < argc)
            temp.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--temp-pattern") == 0 && i + 1 < argc)
            temp.pattern = argv[++i];
        else if (strcmp(argv[i], "--temp-file") == 0 && i + 1 < argc)
            temp.file = argv[++i];

        // --- DISK ---
        else if (strcmp(argv[i], "--disk-enabled") == 0)
            disk.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--disk-time") == 0 && i + 1 < argc)
            disk.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--disk-pattern") == 0 && i + 1 < argc)
            disk.pattern = argv[++i];

        // --- VOLUME ---
        else if (strcmp(argv[i], "--vol-enabled") == 0)
            volume.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--vol-time") == 0 && i + 1 < argc)
            volume.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--vol-pattern") == 0 && i + 1 < argc)
            volume.pattern = argv[++i];

        // --- NETWORK ---
        else if (strcmp(argv[i], "--net-enabled") == 0)
            network.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--net-time") == 0 && i + 1 < argc)
            network.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--net-pattern") == 0 && i + 1 < argc)
            network.pattern = argv[++i];

        // --- DATE ---
        else if (strcmp(argv[i], "--date-enabled") == 0)
            date.enabled = strcmp(argv[++i], "true") == 0;
        else if (strcmp(argv[i], "--date-time") == 0 && i + 1 < argc)
            date.time = atoi(argv[++i]);
        else if (strcmp(argv[i], "--date-pattern") == 0 && i + 1 < argc)
            date.pattern = argv[++i];
    }

    Group groups[9] = {cpu,     ram,    temp,    disk, volume,
                       network, script, battery, date};

    int last_enabled = 0;
    for (int i = 0; i < 9; i++)
        if (groups[i].enabled) last_enabled = i;

    int count = 0;
    for (int i = 0; i < 9; i++) {
        if (groups[i].enabled) {
            Group g = groups[i];

            components[count].wait_ms = groups[i].time;
            components[count].file = groups[i].file;

            if (i != last_enabled) {
                snprintf(components[count].pattern,
                         sizeof(components[count].pattern), "%s %s ", g.pattern,
                         separator);
            } else {
                snprintf(components[count].pattern,
                         sizeof(components[count].pattern), "%s", g.pattern);
            }

            switch (g.type) {
            case Cpu:
                components[count].fn = get_cpu;
                break;
            case Ram:
                components[count].fn = get_ram;
                break;
            case Temperature:
                components[count].fn = get_temperature;
                break;
            case Disk:
                components[count].fn = get_disk;
                break;
            case Volume:
                components[count].fn = get_volume;
                break;
            case Network:
                components[count].fn = get_network;
                break;
            case Date:
                components[count].fn = get_date;
                break;
            case Battery:
                components[count].fn = get_battery;
                break;
            case Script:
                components[count].fn = execute;
                break;
            default:
                perror("Invalid enum");
                exit(1);
            }

            count++;
        }
    }
    return count;
}

bool must_generate(Component *c) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    if (!c->result) {
        c->last_time = current_time;
        return true;
    }

    uint64_t elapsed_ms =
        ((current_time.tv_sec - c->last_time.tv_sec) * 1000) +
        ((current_time.tv_usec - c->last_time.tv_usec) / 1000);

    if (elapsed_ms >= c->wait_ms) {
        c->last_time = current_time;
        return true;
    }

    return false;
}

void msleep(long msec) {
    struct timespec ts = {
        .tv_sec = msec / 1000,
        .tv_nsec = (msec % 1000) * 1000000L,
    };
    thrd_sleep(&ts, NULL);
}
