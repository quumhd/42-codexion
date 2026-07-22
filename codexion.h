
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/time.h>

#define EDF 1
#define FIFO 0

typedef struct s_coder t_coder;
typedef struct s_dongle t_dongle;

typedef struct  s_arguments {
    int             number_of_coders;
    long            time_to_burnout;
    long            time_to_compile;
    long            time_to_debug;
    long            time_to_refactor;
    int             number_of_compiles_required;
    long            dongle_cooldown;
    int             scheduler;

    long             start_time;
    t_coder         *coders;
    t_dongle        *dongles;

    bool            stop;
    pthread_mutex_t stop_lock;
    pthread_mutex_t log_lock;
}   t_arguments;

typedef struct  s_queue {
    int     coder_id;
    long    arrival_time;
    long    deadline;
    bool    active;
}   t_queue;

typedef struct  s_dongle {
    int             id;
    bool            in_use;
    long            released_at;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    t_queue         queue[2];
}   t_dongle;

typedef struct  s_coder {
    int             id;
    pthread_t       thread;
    t_dongle        *left;
    t_dongle        *right;
    long            last_compile_start;
    int             number_of_finished_compiles;
    pthread_mutex_t lock;
    t_arguments     *arguments;
}   t_coder;




long        get_ms_time();
t_arguments	parse_arguments(char **argv);
int	        check_arguments(t_arguments input_args);
void	    print_args_struct(t_arguments input_args);


int         init_coders(t_arguments *args);
int         init_dongles(t_arguments *args);
void	    distribute_dongles(t_arguments *args);
