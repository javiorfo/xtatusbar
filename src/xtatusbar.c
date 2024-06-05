#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "config.h"

#define MAX_STRING_LENGTH 100
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t comp_size = ARRAY_LENGTH(components);

void *thread_component(void *arg) {
    char* result;
    Component *comp = (Component*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        result = comp->fn();
        sprintf(comp->result, comp->head, result);
        pthread_mutex_unlock(&mutex);
        usleep(comp->time);
        if (result != NULL) free(result);
    }
    return NULL;
}

void *thread_principal(void *arg) {
    char final_result[MAX_STRING_LENGTH];
    while (1) {
        char final[MAX_STRING_LENGTH] = "";
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < comp_size; i++) {
            strcat(final, components[i].result);
        }
        sprintf(final_result, "%s", final);
        pthread_mutex_unlock(&mutex);
        // TODO call xrootset
        printf("%s\n", final_result);
        usleep(100000);
    }
    return NULL;
}

int main() {
    pthread_t thread_final;
    pthread_t threads[comp_size];

    for (int i = 0; i < comp_size; i++) {
        pthread_create(&threads[i], NULL, thread_component, &components[i]);
    }
    pthread_create(&thread_final, NULL, thread_principal, NULL);
    
    for (int i = 0; i < comp_size; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(thread_final, NULL);

    return 0;
}

char* get_cpu_temperature() {
//     FILE *thermal_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    FILE *thermal_file = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input", "r");
    if (thermal_file == NULL) {
        perror("Error opening thermal file");
        exit(EXIT_FAILURE);
    }

    int temperature;
    fscanf(thermal_file, "%d", &temperature);
    fclose(thermal_file);
    
    int temp = temperature / 1000;
   
    char* str = (char*)malloc(10 * sizeof(char));
    if (str == NULL) return NULL; 

    sprintf(str, "%d", temp);

    return str;
}

char* get_cpu_usage() {
    struct sysinfo info;
    if(sysinfo(&info) != 0) {
        return "";
    }
    
    double total_cpu_time = info.totalram - info.freeram;
    int cpu_usage = (total_cpu_time / info.totalram) * 100;
    
    char* str = (char*)malloc(10 * sizeof(char));
    if (str == NULL) return NULL; 
    
    sprintf(str, "%d", cpu_usage);

    return str;
}
