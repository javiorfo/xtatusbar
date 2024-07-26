#include <stdio.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "config.h"

#define STATUSBAR_MAX_STRING_LENGTH 200
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]));
#define MILISECONDS_TO_MICROSECONDS(ms) ms * 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static const size_t COMP_COUNT = ARRAY_LENGTH(components);

void* thread_component(void *arg) {
    Component* component = (Component*)arg;

    while (true) {
        pthread_mutex_lock(&mutex);

        component->result = component->fn(component->head);
        if (component->result == NULL) {
            pthread_mutex_unlock(&mutex);
            continue;
        }

        pthread_mutex_unlock(&mutex);
        
        usleep(MILISECONDS_TO_MICROSECONDS(component->time));

        pthread_mutex_lock(&mutex);

        if (component->result != NULL) {
            free(component->result);
            component->result = NULL;
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* thread_bar(void *arg) {
    while (true) {
        char final_str[STATUSBAR_MAX_STRING_LENGTH] = "xsetroot -name \"";

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < COMP_COUNT; i++) {
            if (components[i].result != NULL)
                strcat(final_str, components[i].result);
        }
        strcat(final_str, "\"");

        pthread_mutex_unlock(&mutex);

        system(final_str);
        usleep(MILISECONDS_TO_MICROSECONDS(100));
    }
    return NULL;
}

int main() {
    pthread_t thread_final;
    pthread_t threads[COMP_COUNT];

    for (int i = 0; i < COMP_COUNT; i++) {
        pthread_create(&threads[i], NULL, thread_component, &components[i]);
    }
    pthread_create(&thread_final, NULL, thread_bar, NULL);
    
    for (int i = 0; i < COMP_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(thread_final, NULL);

    return 0;
}

char* build_result_for_short(char* head, short value, short size) {
    int final_size = strlen(head) + size;
    char* str = (char*)malloc(final_size * sizeof(char));
    if (str == NULL) return NULL; 

    sprintf(str, head, value);

    return str;
}

char* build_result_for_string(char* head, char* value, short size) {
    int final_size = strlen(head) + size;
    char* str = (char*)malloc(final_size * sizeof(char));
    if (str == NULL) return NULL; 

    sprintf(str, head, value);

    return str;
}

char* get_cpu_temperature(char* head) {
    FILE* thermal_file = fopen(TEMPERATURE_FILE, "r");
    if (thermal_file == NULL) {
        perror("Error opening thermal file");
        return build_result_for_string("TEMP s%", "-", 1);
    }

    int temperature;
    fscanf(thermal_file, "%d", &temperature);
    fclose(thermal_file);
    
    short temp = temperature / 1000;

    return build_result_for_short(head, temp, 3);
}

char* get_cpu_usage(char* head) {
    struct sysinfo info;
    if(sysinfo(&info) != 0) {
        perror("Error getting sysinfo");
        return build_result_for_string("CPU s%", "-", 1);
    }

    double total_cpu_time = info.totalram - info.freeram;
    short used_cpu = (total_cpu_time / info.totalram) * 100;
    
    return build_result_for_short(head, used_cpu, 3);
}

char* get_ram_usage(char* head) {
    FILE *fp;
    char line[1024];
    unsigned long total_mem, free_mem, buffers, cached, used_mem;
    short used_ram_perc;

    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        printf("Failed to open /proc/meminfo\n");
        return build_result_for_string("RAM s%", "-", 1);
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
    return build_result_for_short(head, used_ram_perc, 3);
}

char* get_disk_usage(char* head) {
    struct statvfs stat;

    if (statvfs("/", &stat) != 0) {
        perror("Error with statvfs");
        return build_result_for_string("DISK s%", "-", 1);
    }

    unsigned long total_blocks = stat.f_blocks;
    unsigned long free_blocks = stat.f_bfree;
    unsigned long used_blocks = total_blocks - free_blocks;

    short used_disk_perc = (double) used_blocks / total_blocks * 100;
    
    return build_result_for_short(head, used_disk_perc, 2);
}

char* get_date(char* head) {
    time_t now;
    struct tm *local;
    char buffer[80];

    time(&now);
    local = localtime(&now);

    strftime(buffer, 80, DATE_FORMAT, local);

    return build_result_for_string(head, buffer, 20);
}

char* network_is_connected(char* head) {
    struct addrinfo hints, *res;
    int status;
    char hostname[] = "www.google.com";
    bool is_connected = true;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0 || res == NULL) {
        is_connected = false;
    }

    freeaddrinfo(res); 

    return build_result_for_string(head, is_connected ? "󰱓 " : "󰅛 ", 3);
}

char* get_volume(char* head) {
    char buffer[1024] = {0};
    FILE *fp;

    fp = popen("pactl get-sink-mute @DEFAULT_SINK@", "r");
    if (fp == NULL) {
        perror("popen failed in pactl get-sink-mute");
        return build_result_for_string(head, "-", 1);
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        perror("fgets failed in pactl get-sink-mute");
        pclose(fp);
        return build_result_for_string(head, "-", 1);
    }

    pclose(fp);
    char muted[] = "";
    if (sscanf(buffer, "Mute: %s", muted) != 1) {
        fprintf(stderr, "Failed to parse volume Mute\n");
        return build_result_for_string(head, "-", 1);
    }

    bool is_muted = strcmp(muted, "yes") == 0 ? true : false;

    if (is_muted) {
        return build_result_for_string(head, "MUTED", 5);
    }
    
    fp = popen("pactl get-sink-volume @DEFAULT_SINK@", "r");
    if (fp == NULL) {
        perror("popen failed");
        return build_result_for_string(head, "-", 1);
    }

    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        perror("fgets failed");
        pclose(fp);
        return build_result_for_string(head, "-", 1);
    }

    pclose(fp);

    const char *volume_str = strstr(buffer, "Volume:");
    if (volume_str == NULL) {
        fprintf(stderr, "Volume information not found in the output\n");
        return build_result_for_string(head, "-", 1);
    }

    int percentage = 0;
    if (sscanf(volume_str, "Volume: %*[^/]/ %d%%", &percentage) != 1) {
        fprintf(stderr, "Failed to parse volume percentage\n");
        return build_result_for_string(head, "-", 1);
    }

    char str[3];
    sprintf(str, "%hd%%", percentage);
    return build_result_for_string(head, str, 3);
}

char* get_battery_status(char* head) {
    const int MAX_BUF = 128;
    FILE *file;
    char buffer[MAX_BUF];
    int battery_percentage;

    file = fopen(BATTERY_FILE, "r");
    if (file == NULL) {
        perror("Error opening file to get battery info");
        return build_result_for_string(head, "-", 1);
    }

    while (fgets(buffer, MAX_BUF, file)) {
        if (strstr(buffer, "POWER_SUPPLY_CAPACITY=") != NULL) {
            sscanf(buffer, "POWER_SUPPLY_CAPACITY=%d", &battery_percentage);
            break;
        }
    }

    fclose(file);
    return build_result_for_short(head, battery_percentage, 3);
}

char* execute_script(char* head) {
    FILE *fp;
    char path[1024];

    fp = popen(SCRIPT, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        return build_result_for_string(head, "-", 1);
    }

    while (fgets(path, sizeof(path), fp) != NULL) {}

    pclose(fp);
    return build_result_for_string(head, path, strlen(path) + 1);
}
