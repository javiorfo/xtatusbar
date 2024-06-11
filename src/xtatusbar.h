typedef char* (*function)(char*);

typedef struct {
    char* head;
    int time;
    function fn;
    char* result;
} Component;

struct sysinfo get_sysinfo();
char* build_result_for_short(char*, short, short);
char* build_result_for_string(char*, char*, short);

char* get_cpu_temperature(char*);
char* get_cpu_usage(char*);
char* get_ram_usage(char*);


