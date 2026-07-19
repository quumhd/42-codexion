/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 13:52:05 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/19 18:10:15 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_args_struct(t_arguments input_args)
{
	printf(
		"number_of_coders: %d\n"
		"time_to_burnout: %ld\n"
		"time_to_compile: %ld\n"
		"time_to_debug: %ld\n"
		"time_to_refactor: %ld\n"
		"number_of_compiles_required: %d\n"
		"dongle_cooldown: %ld\n"
		"time of day: %ld\n"
		"scheduler: %d\n",
		input_args.number_of_coders,
		input_args.time_to_burnout,
		input_args.time_to_compile,
		input_args.time_to_debug,
		input_args.time_to_refactor,
		input_args.number_of_compiles_required,
		input_args.dongle_cooldown,
		input_args.start_time,
		input_args.scheduler
		);
}

t_arguments	parse_arguments(char **argv)
{
	struct timeval tv;
	t_arguments	input_args;

	input_args.number_of_coders = atoi(argv[1]);
	input_args.time_to_burnout = (long) atoi(argv[2]);
	input_args.time_to_compile = (long) atoi(argv[3]);
	input_args.time_to_debug = (long) atoi(argv[4]);
	input_args.time_to_refactor = (long) atoi(argv[5]);
	input_args.number_of_compiles_required = atoi(argv[6]);
	input_args.dongle_cooldown = (long) atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		input_args.scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		input_args.scheduler = EDF;
	else
		input_args.scheduler = 3;
	input_args.start_time = get_ms_time();
	return (input_args);
}

int	check_arguments(t_arguments input_args)
{
	if (input_args.number_of_coders <= 0)
		return (fprintf (stderr, "Number of coders cannot be zero or negative\n"), -1);
	if (input_args.time_to_burnout <= 0)
		return (fprintf (stderr, "Time to burnout cannot be zero or negative\n"), -1);
	if (input_args.time_to_compile <= 0)
		return (fprintf (stderr, "Time to compile cannot be zero or negative\n"), -1);
	if (input_args.time_to_debug <= 0)
		return (fprintf (stderr, "Time to debug cannot be zero or negative\n"), -1);
	if (input_args.time_to_refactor <= 0)
		return (fprintf (stderr, "Time to refactor cannot be zero or negative\n"), -1);
	if (input_args.number_of_compiles_required <= 0)
		return (fprintf (stderr, "Number of compiles required coders cannot be zero or negative\n"), -1);
	if (input_args.dongle_cooldown <= 0)
		return (fprintf (stderr, "Dongle cooldown cannot be zero or negative\n"), -1);
	if (input_args.scheduler == 3)
		return (fprintf (stderr, "Scheduler can only be \"fifo\" or \"edf\"\n"), -1);
	return (0);
}

int init_dongles(t_arguments *args)
{
	int	dongle_id;

	dongle_id = 0;
	args->dongles = malloc(sizeof(t_dongle) * args->number_of_coders);
	if (!args->dongles)
		return (-1);
	while (dongle_id < args->number_of_coders)
	{
		args->dongles[dongle_id].id = dongle_id + 1;
		args->dongles[dongle_id].in_use = 0;
		args->dongles[dongle_id].released_at = -1;
		if (pthread_mutex_init(&args->dongles[dongle_id].lock, NULL) != 0)
			return (-1);
		if (pthread_cond_init(&args->dongles[dongle_id].cond, NULL) != 0)
			return (-1);
		dongle_id++;
	}
	return (0);
}

void	distribute_dongles(t_arguments *args)
{
	int i;
	int noc;

	i = 0;
	noc = args->number_of_coders;
	while (i < noc)
	{
		if (noc == 1)
		{
			args->coders[0].right = &args->dongles[0];
			args->coders[0].left = &args->dongles[0];
		}
		else
		{
            args->coders[i].right = &args->dongles[i];
			if (i != 0)
				args->coders[i].left = &args->dongles[i - 1];
			else
				args->coders[i].left = &args->dongles[noc - 1];
		}
		i++;
	}
}

int	init_coders(t_arguments *args)
{
	int	coder_id;

	coder_id = 0;
	args->coders = malloc(sizeof(t_coder) * args->number_of_coders);
	if (!args->coders)
		return (-1);
	while (coder_id < args->number_of_coders)
	{
		args->coders[coder_id].id = coder_id + 1;
		args->coders[coder_id].last_compile_start = get_ms_time();
		args->coders[coder_id].number_of_finished_compiles = 0;
		args->coders[coder_id].arguments = args;
		if (pthread_mutex_init(&args->coders[coder_id].lock, NULL) != 0)
			return (-1);
		coder_id++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_arguments	*args;


	if (argc != 9)
		return (fprintf(stderr, "Wrong input\n"), 1);
	args = malloc(sizeof(t_arguments));
	*args = parse_arguments(argv);
	if (check_arguments(*args) == -1)
		return (1);
	if (init_dongles(args) == -1)
		return (1);
	if (init_coders(args) == -1)
		return (1);
	distribute_dongles(args);
	print_args_struct(*args);
	return (0);
}
