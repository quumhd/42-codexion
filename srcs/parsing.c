/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:10:34 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/23 14:52:04 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#define NUMBER_OF_CODERS "Number of coders needs to be a positive integer\n"
#define TIME_TO_BURNOUT "Time to burnout needs to be a positive integer\n"
#define TIME_TO_COMPILE "Time to compile needs to be a positive integer\n"
#define TIME_TO_DEBUG "Time to debug needs to be a positive integer\n"
#define TIME_TO_REFACTOR "Time to refactor needs to be a positive integer\n"
#define N_O_C_R "Number of compiles required needs to be a positive integer\n"
#define DONGLE_COOLDOWN "Dongle cooldown needs to be a positive integer\n"
#define SCHEDULER "Scheduler can only be \"fifo\" or \"edf\"\n"

bool	is_numberical(char s)
{
	if (s >= '0' && s <= '9')
		return (true);
	return (false);
}

bool	contains_only_numbers(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (is_numberical(s[i]) == false)
			return (false);
		i++;
	}
	return (true);
}

void	check_non_numberical(char **argv, t_arguments *input_args)
{
	if (contains_only_numbers(argv[1]) == false)
		input_args->number_of_coders = -1;
	if (contains_only_numbers(argv[2]) == false)
		input_args->time_to_burnout = -1;
	if (contains_only_numbers(argv[3]) == false)
		input_args->time_to_compile = -1;
	if (contains_only_numbers(argv[4]) == false)
		input_args->time_to_debug = -1;
	if (contains_only_numbers(argv[5]) == false)
		input_args->time_to_refactor = -1;
	if (contains_only_numbers(argv[6]) == false)
		input_args->number_of_compiles_required = -1;
	if (contains_only_numbers(argv[7]) == false)
		input_args->dongle_cooldown = -1;
}

t_arguments	parse_arguments(char **argv)
{
	t_arguments		input_args;

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
	check_non_numberical(argv, &input_args);
	return (input_args);
}

int	check_arguments(t_arguments input_args)
{
	if (input_args.number_of_coders < 0)
		return (fprintf (stderr, NUMBER_OF_CODERS), -1);
	if (input_args.time_to_burnout < 0)
		return (fprintf (stderr, TIME_TO_BURNOUT), -1);
	if (input_args.time_to_compile < 0)
		return (fprintf (stderr, TIME_TO_COMPILE), -1);
	if (input_args.time_to_debug < 0)
		return (fprintf (stderr, TIME_TO_DEBUG), -1);
	if (input_args.time_to_refactor < 0)
		return (fprintf (stderr, TIME_TO_REFACTOR), -1);
	if (input_args.number_of_compiles_required < 0)
		return (fprintf (stderr, N_O_C_R), -1);
	if (input_args.dongle_cooldown < 0)
		return (fprintf (stderr, DONGLE_COOLDOWN), -1);
	if (input_args.scheduler == 3)
		return (fprintf (stderr, SCHEDULER), -1);
	return (0);
}
