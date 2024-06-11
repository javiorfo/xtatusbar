#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
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

        if (component->result != NULL) free(component->result);
        component->result = NULL;
    }
    return NULL;
}

void* thread_bar(void *arg) {
    while (true) {
        char final_str[STATUSBAR_MAX_STRING_LENGTH] = "";

        pthread_mutex_lock(&mutex);

        for (int i = 0; i < COMP_COUNT; i++) {
            if (components[i].result != NULL)
                strcat(final_str, components[i].result);
        }

        pthread_mutex_unlock(&mutex);
        // TODO call xrootset
        printf("%s\n", final_str);
        usleep(MILISECONDS_TO_MICROSECONDS(10));
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

struct sysinfo get_sysinfo() {
    struct sysinfo info;
    if(sysinfo(&info) != 0) {
        perror("Error getting sysinfo");
        exit(EXIT_FAILURE);
    }
    return info;
}

char* build_result_for_short(char* head, short value, short size) {
    char* str = (char*)malloc(size * sizeof(char));
    if (str == NULL) return NULL; 

    sprintf(str, head, value);

    return str;
}

char* build_result_for_string(char* head, char* value, short size) {
    char* str = (char*)malloc(size * sizeof(char));
    if (str == NULL) return NULL; 

    sprintf(str, head, value);

    return str;
}

char* get_cpu_temperature(char* head) {
    FILE* thermal_file = fopen(TEMPERATURE_FILE, "r");
    if (thermal_file == NULL) {
        perror("Error opening thermal file");
        exit(EXIT_FAILURE);
    }

    int temperature;
    fscanf(thermal_file, "%d", &temperature);
    fclose(thermal_file);
    
    short temp = temperature / 1000;

    return build_result_for_short(head, temp, 20);
}

char* get_cpu_usage(char* head) {
    struct sysinfo info = get_sysinfo();
    
    double total_cpu_time = info.totalram - info.freeram;
    short used_cpu = (total_cpu_time / info.totalram) * 100;
    
    return build_result_for_short(head, used_cpu, 20);
}

char* get_ram_usage(char* head) {
    struct sysinfo info = get_sysinfo();
    
    unsigned long total_ram = info.totalram * info.mem_unit;
    unsigned long free_ram = info.freeram * info.mem_unit;
    unsigned long used_ram = total_ram - free_ram;
    short used_ram_perc = ((double)used_ram / total_ram) * 100;
    
    return build_result_for_short(head, used_ram_perc, 20);
}
