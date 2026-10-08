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

#define INT_LIMIT 2147483647L
#define NUMBER_OF_CODERS "Number of coders needs to be a positive integer\n"
#define TIME_TO_BURNOUT "Time to burnout needs to be a positive integer\n"
#define TIME_TO_COMPILE "Time to compile needs to be a positive integer\n"
#define TIME_TO_DEBUG "Time to debug needs to be a non-negative integer\n"
#define TIME_TO_REFACTOR "Time to refactor needs to be a non-negative integer\n"
#define N_O_C_R "Number of compiles required needs to be a positive integer\n"
#define DONGLE_COOLDOWN "Dongle cooldown needs to be a non-negative integer\n"
#define SCHEDULER "Scheduler can only be \"fifo\" or \"edf\"\n"

long	parse_number(char *s)
{
	int		i;
	long	nbr;

	i = 0;
	nbr = 0;
	if (!s[0])
		return (-1);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (-1);
		nbr = nbr * 10 + (s[i] - '0');
		if (nbr > INT_LIMIT)
			return (-1);
		i++;
	}
	return (nbr);
}

t_arguments	parse_arguments(char **argv)
{
	t_arguments		input_args;

	input_args.number_of_coders = (int) parse_number(argv[1]);
	input_args.time_to_burnout = parse_number(argv[2]);
	input_args.time_to_compile = parse_number(argv[3]);
	input_args.time_to_debug = parse_number(argv[4]);
	input_args.time_to_refactor = parse_number(argv[5]);
	input_args.number_of_compiles_required = (int) parse_number(argv[6]);
	input_args.dongle_cooldown = parse_number(argv[7]);
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
	if (input_args.number_of_coders < 1)
		return (fprintf (stderr, NUMBER_OF_CODERS), -1);
	if (input_args.time_to_burnout < 1)
		return (fprintf (stderr, TIME_TO_BURNOUT), -1);
	if (input_args.time_to_compile < 1)
		return (fprintf (stderr, TIME_TO_COMPILE), -1);
	if (input_args.time_to_debug < 0)
		return (fprintf (stderr, TIME_TO_DEBUG), -1);
	if (input_args.time_to_refactor < 0)
		return (fprintf (stderr, TIME_TO_REFACTOR), -1);
	if (input_args.number_of_compiles_required < 1)
		return (fprintf (stderr, N_O_C_R), -1);
	if (input_args.dongle_cooldown < 0)
		return (fprintf (stderr, DONGLE_COOLDOWN), -1);
	if (input_args.scheduler == 3)
		return (fprintf (stderr, SCHEDULER), -1);
	return (0);
}
