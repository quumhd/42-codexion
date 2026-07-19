/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 13:54:20 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/19 17:44:34 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#define FIFO 0
#define EDF 1

typedef struct s_dongle t_dongle;
typedef struct s_coder t_coder;

typedef struct s_arguments {
    int         number_of_coders;
    long        time_to_burnout;
    long        time_to_compile;
    long        time_to_debug;
    long        time_to_refactor;
    int         number_of_compiles_required;
    long        dongle_cooldown;
    int         scheduler;

    long         start_time;
    t_coder     *coders;
    t_dongle    *dongles;

    int         stop;
    pthread_mutex_t stop_lock;
    pthread_mutex_t log_lock;
}   t_arguments;


typedef struct s_dongle {
    int             id;
    int             in_use;
    long            released_at;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    // t_waiter        *waiters;
}   t_dongle;

typedef struct s_coder {
    int             id;
    pthread_t       thread;
    t_dongle        *left;
    t_dongle        *right;
    long            last_compile_start;
    int             number_of_finished_compiles;
    pthread_mutex_t lock;
    t_arguments     *arguments;
}   t_coder;


long get_ms_time();
