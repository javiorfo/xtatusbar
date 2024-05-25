#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LENGTH 1000

int temperature(char[]);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char string_one[MAX_STRING_LENGTH] = "";
char string_temperature[MAX_STRING_LENGTH] = "";

void *thread_custom(void *arg) {
    int count = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        sprintf(string_one, "Uno %d", count++);
        pthread_mutex_unlock(&mutex);
        usleep(1000000); // Sleep for 1 second
    }
    return NULL;
}

void *thread1_function(void *arg) {
    int count = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        sprintf(string_one, "Uno %d", count++);
//         strcpy(thread1_result, str);
        pthread_mutex_unlock(&mutex);
        usleep(1000000); // Sleep for 1 second
    }
    return NULL;
}

void *thread2_function(void *arg) {
    char con[1000];
    while (1) {
        pthread_mutex_lock(&mutex);
        temperature(con);
        sprintf(string_temperature, "Dos %s", con);
        pthread_mutex_unlock(&mutex);
        usleep(500000); // Sleep for 100 milliseconds
    }
    return NULL;
}

void *thread_final(void *arg) {
    char final_result[MAX_STRING_LENGTH];
    while (1) {
        pthread_mutex_lock(&mutex);
        sprintf(final_result, "%s %s", string_one, string_temperature);
        pthread_mutex_unlock(&mutex);
        printf("Concatenated result: %s\n", final_result);
        usleep(100000); // Sleep for 10 milliseconds
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2, thread_final1;

    // Create threads
    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);
    pthread_create(&thread_final1, NULL, thread_final, NULL);

    // Join threads
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread_final1, NULL);

    return 0;
}

int temperature(char con[]) {
    FILE *fp;                    // declaration of file pointer
    fp = fopen("/sys/devices/platform/coretemp.0/hwmon/hwmon1/temp1_input", "r"); // opening of file
    if (!fp)                     // checking of error
      return 1;

    con = fgets(con,1000, fp);// reading file content

    fclose(fp);               // closing of file
    return 0;
}
