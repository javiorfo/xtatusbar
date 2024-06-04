typedef int (*function_int)();
typedef char* (*function_string)();

typedef struct {
    char *head;
    int time;
    function_int fn_int;
    char result[10];
} Component;

int get_cpu_temperature();
int test();
int get_cpu_usage();


