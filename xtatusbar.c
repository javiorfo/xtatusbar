#include "config.h"
#include <alsa/asoundlib.h>
#include <curl/curl.h>
#include <net/if.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

#define STATUSBAR_MAX_STRING_LENGTH 200
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]));
#define MILISECONDS_TO_MICROSECONDS(ms) ms * 1000
#define BUFFER(str, plus) strlen(str) + plus

static const size_t COMP_COUNT = ARRAY_LENGTH(components);

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

void disk(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 2))) {
        c->result = "-";
    } else if (must_generate(c)) {
        struct statvfs stat;
        if (statvfs("/", &stat) != 0) {
            perror("Error with statvfs");
            c->result = "-";
            return;
        }

        unsigned long total_blocks = stat.f_blocks;
        unsigned long free_blocks = stat.f_bfree;
        unsigned long used_blocks = total_blocks - free_blocks;

        short used_disk_perc = (double)used_blocks / total_blocks * 100;

        sprintf(c->result, c->pattern, used_disk_perc);
    }
}

void date(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 50))) {
        c->result = "-";
    } else if (must_generate(c)) {
        time_t now;
        struct tm *local;

        time(&now);
        local = localtime(&now);

        strftime(c->result, 80, DATE_FORMAT, local);
    }
}

void ram(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 2))) {
        c->result = "-";
    } else if (must_generate(c)) {
        FILE *fp;
        char line[1024];
        unsigned long total_mem, free_mem, buffers, cached, used_mem;
        short used_ram_perc;

        fp = fopen("/proc/meminfo", "r");
        if (fp == NULL) {
            perror("Failed to open /proc/meminfo\n");
            c->result = "-";
            return;
        }

        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "MemTotal: %lu kB", &total_mem) == 1) {
                continue;
            } else if (sscanf(line, "MemFree: %lu kB", &free_mem) == 1) {
                continue;
            } else if (sscanf(line, "Buffers: %lu kB", &buffers) == 1) {
                continue;
            } else if (sscanf(line, "Cached: %lu kB", &cached) == 1) {
                break;
            }
        }

        fclose(fp);

        used_mem = total_mem - free_mem - buffers - cached;

        used_ram_perc = ((double)used_mem / total_mem) * 100;

        sprintf(c->result, c->pattern, used_ram_perc);
    }
}

void network(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 2))) {
        c->result = "-";
    } else if (must_generate(c)) {
        int status = system("ping -c 1 -q 8.8.8.8 > /dev/null 2>&1");
        sprintf(c->result, c->pattern, (status == 0 ? "󰱓 " : "󰅛 "));
    }
}

void temperature(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 2))) {
        c->result = "-";
    } else if (must_generate(c)) {
        FILE *thermal_file = fopen(TEMPERATURE_FILE, "r");
        if (thermal_file == NULL) {
            perror("Error opening thermal file");
            c->result = "-";
            return;
        }

        int temperature;
        fscanf(thermal_file, "%d", &temperature);
        fclose(thermal_file);

        short temp = temperature / 1000;

        sprintf(c->result, c->pattern, temp);
    }
}

void cpu(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 2))) {
        c->result = "-";
    } else if (must_generate(c)) {
        struct sysinfo info;
        if (sysinfo(&info) != 0) {
            perror("Error getting sysinfo");
            c->result = "-";
            return;
        }

        double total_cpu_time = info.totalram - info.freeram;
        short used_cpu = (total_cpu_time / info.totalram) * 100;

        sprintf(c->result, c->pattern, used_cpu);
    }
}

void volume(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 5))) {
        c->result = "-";
    } else if (must_generate(c)) {
        long min, max, volume;
        snd_mixer_t *handle;
        snd_mixer_selem_id_t *sid;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_alloca(&sid);
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, selem_name);
        snd_mixer_elem_t *elem = snd_mixer_find_selem(handle, sid);

        int muted;
        snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_MONO, &muted);

        if (muted == 0) {
            sprintf(c->result, c->pattern, "MUTED");
        } else {
            snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
            snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO,
                                                &volume);
            snd_mixer_close(handle);

            int percentage = (int)((volume - min) * 100 / (max - min));

            char str[3];
            sprintf(str, "%hd%%", percentage);

            sprintf(c->result, c->pattern, str);
        }
    }
}

void battery(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 5))) {
        c->result = "-";
    } else if (must_generate(c)) {
        const int MAX_BUF = 128;
        FILE *file;
        char buffer[MAX_BUF];
        int battery_percentage;

        file = fopen(BATTERY_FILE, "r");
        if (file == NULL) {
            perror("Error opening file to get battery info");
            c->result = "-";
            return;
        }

        while (fgets(buffer, MAX_BUF, file)) {
            if (strstr(buffer, "POWER_SUPPLY_CAPACITY=") != NULL) {
                sscanf(buffer, "POWER_SUPPLY_CAPACITY=%d", &battery_percentage);
                break;
            }
        }

        fclose(file);

        sprintf(c->result, c->pattern, battery_percentage);
    }
}

void script(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 80))) {
        c->result = "-";
    } else if (must_generate(c)) {
        FILE *fp;
        char path[80];

        fp = popen(SCRIPT, "r");
        if (fp == NULL) {
            perror("Failed to run command\n");
            c->result = "-";
            return;
        }

        while (fgets(path, sizeof(path), fp) != NULL) {}

        pclose(fp);

        sprintf(c->result, c->pattern, path);
    }
}

int main() {
    while (true) {
        char final_str[200] = "xsetroot -name \"";
        for (int i = 0; i < COMP_COUNT; i++) {
            Component *c = components + i;
            c->fn(c);
            strcat(final_str, c->result);
        }
        strcat(final_str, "\"");

        system(final_str);
        usleep(100000);
    }

    return 0;
}
