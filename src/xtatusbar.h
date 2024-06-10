typedef char* (*function)(char*);

typedef struct {
    char* head;
    int time;
    function fn;
    char* result;
} Component;

char* get_cpu_temperature(char*);
char* get_cpu_usage(char*);
char* get_ram_usage(char*);


