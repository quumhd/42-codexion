/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:14:15 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/20 15:14:45 by jdreissi         ###   ########.fr       */
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