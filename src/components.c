#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

double get_cpu_usage() {
    struct sysinfo info;
    if(sysinfo(&info) != 0) {
        return -1; // Error occurred
    }
    
    double total_cpu_time = info.totalram - info.freeram;
    double cpu_usage = (total_cpu_time / info.totalram) * 100;
    
    return cpu_usage;
}

float get_cpu_temperature() {
    FILE *thermal_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (thermal_file == NULL) {
        perror("Error opening thermal file");
        exit(EXIT_FAILURE);
    }

    int temperature;
    fscanf(thermal_file, "%d", &temperature);
    fclose(thermal_file);

    return temperature / 1000.0; // Convert millidegrees to degrees Celsius
}


int main() {
    double cpu_usage = get_cpu_usage();
    
    if(cpu_usage == -1) {
        printf("Error getting CPU usage\n");
    } else {
        printf("CPU Usage: %.2f%%\n", cpu_usage);
    }

    float cpu_temperature = get_cpu_temperature();

    printf("CPU Temperature: %.2f°C\n", cpu_temperature);
    
    return 0;
}

