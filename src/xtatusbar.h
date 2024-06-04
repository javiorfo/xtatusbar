typedef int (*FunctionInt)();
typedef char* (*FunctionString)();

typedef struct {
    char *head;
    int time;
    FunctionInt function_int;
    char result[10];
} Component;

int get_cpu_temperature();
int test();
int get_cpu_usage();


