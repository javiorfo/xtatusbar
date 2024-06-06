typedef char* (*function)();

typedef struct {
    char *head;
    int time;
    function fn;
    char *result;
} Component;

char* get_cpu_temperature();
char* get_cpu_usage();


