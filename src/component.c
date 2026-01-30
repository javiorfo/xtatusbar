#include "util.h"
#include <alsa/asoundlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>

void get_disk(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 2;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        struct statvfs stat;
        if (statvfs("/", &stat) != 0) {
            perror("Error with statvfs");
            return;
        }

        unsigned long total_blocks = stat.f_blocks;
        unsigned long free_blocks = stat.f_bfree;
        unsigned long used_blocks = total_blocks - free_blocks;

        short used_disk_perc = (double)used_blocks / total_blocks * 100;

        snprintf(c->result, c->pattern_len, c->pattern, used_disk_perc);
    }
}

void get_date(Component *c) {
    if (!c->result && !(c->result = malloc(strlen(c->pattern) + 50))) return;

    if (must_generate(c)) {
        time_t now;
        struct tm *local;

        time(&now);
        local = localtime(&now);

        strftime(c->result, 80, c->pattern, local);
    }
}

void get_ram(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 2;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        FILE *fp;
        char line[1024];
        unsigned long total_mem, free_mem, buffers, cached, used_mem;
        short used_ram_perc;

        fp = fopen("/proc/meminfo", "r");
        if (fp == NULL) {
            perror("Failed to open /proc/meminfo\n");
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

        snprintf(c->result, c->pattern_len, c->pattern, used_ram_perc);
    }
}

bool check_connection() {
    struct sockaddr_in serv_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) return false;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return false;
    }

    close(sock);
    return true;
}

void get_network(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 4;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        snprintf(c->result, c->pattern_len, c->pattern,
                 (check_connection() ? "󰀂 " : "󰯡 "));
    }
}

void get_temperature(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 2;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        FILE *thermal_file = fopen(c->file, "r");
        if (thermal_file == NULL) {
            perror("Error opening thermal file");
            return;
        }

        int temperature;
        fscanf(thermal_file, "%d", &temperature);
        fclose(thermal_file);

        short temp = temperature / 1000;

        snprintf(c->result, c->pattern_len, c->pattern, temp);
    }
}

void get_stats(long long *idle, long long *total) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        perror("Could not open /proc/stat");
        return;
    }

    long long user, nice, system, idle_val, iowait, irq, softirq, steal;
    fscanf(fp, "cpu %lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
           &system, &idle_val, &iowait, &irq, &softirq, &steal);

    *idle = idle_val + iowait;
    *total = user + nice + system + idle_val + iowait + irq + softirq + steal;

    fclose(fp);
}

void get_cpu(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 2;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        long long idle1, total1, idle2, total2;

        get_stats(&idle1, &total1);
        msleep(100);
        get_stats(&idle2, &total2);

        long long total_diff = total2 - total1;
        long long idle_diff = idle2 - idle1;

        short usage = (double)(total_diff - idle_diff) / total_diff * 100.0;

        snprintf(c->result, c->pattern_len, c->pattern, usage);
    }
}

void get_volume(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 2;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        long min, max, volume;
        snd_mixer_t *handle;
        snd_mixer_selem_id_t *sid;
        const char *card = "default";
        const char *selem_name = "Master";

        snd_mixer_open(&handle, 0);
        snd_mixer_attach(handle, card);
        snd_mixer_selem_register(handle, NULL, NULL);
        snd_mixer_load(handle);

        snd_mixer_selem_id_malloc(&sid);
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
            snd_mixer_selem_id_free(sid);
            snd_mixer_close(handle);

            int percentage = (int)((volume - min) * 100 / (max - min));

            char str[3];
            sprintf(str, "%hd%%", percentage);

            snprintf(c->result, c->pattern_len, c->pattern, str);
        }
    }
}

void get_battery(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 5;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        FILE *fp;
        int capacity;

        fp = fopen(c->file, "r");

        if (fp == NULL) {
            perror("Error opening battery capacity file");
            return;
        }

        if (fscanf(fp, "%d", &capacity) != 1) {
            fprintf(stderr, "Error reading capacity\n");
            fclose(fp);
            return;
        }

        fclose(fp);

        snprintf(c->result, c->pattern_len, c->pattern, capacity);
    }
}

void execute(Component *c) {
    if (!c->pattern_len) c->pattern_len = strlen(c->pattern) + 20;

    if (!c->result && !(c->result = malloc(c->pattern_len))) return;

    if (must_generate(c)) {
        FILE *fp;
        char path[80];

        fp = popen(c->file, "r");
        if (fp == NULL) {
            perror("Failed to run command\n");
            return;
        }

        while (fgets(path, sizeof(path), fp) != NULL) {}

        pclose(fp);

        snprintf(c->result, c->pattern_len, c->pattern, path);
    }
}
