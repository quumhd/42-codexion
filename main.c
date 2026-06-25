/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 13:52:05 by jdreissi          #+#    #+#             */
/*   Updated: 2026/06/25 14:12:23 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	print_args_struct(input_args input_args)
{
	printf(
		"number_of_coders: %d\n"
		"time_to_burnout: %d\n"
		"time_to_compile: %d\n"
		"time_to_debug: %d\n"
		"time_to_refactor: %d\n"
		"number_of_compiles_required: %d\n"
		"dongle_cooldown: %d\n"
		"scheduler: %d\n",
		input_args.number_of_coders,
		input_args.time_to_burnout,
		input_args.time_to_compile,
		input_args.time_to_debug,
		input_args.time_to_refactor,
		input_args.number_of_compiles_required,
		input_args.dongle_cooldown,
		input_args.scheduler
		);
}

input_args	parse_arguments(char **argv)
{
	input_args	input_args;

	input_args.number_of_coders = atoi(argv[1]);
	input_args.time_to_burnout = atoi(argv[2]);
	input_args.time_to_compile = atoi(argv[3]);
	input_args.time_to_debug = atoi(argv[4]);
	input_args.time_to_refactor = atoi(argv[5]);
	input_args.number_of_compiles_required = atoi(argv[6]);
	input_args.dongle_cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		input_args.scheduler = FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		input_args.scheduler = EDF;
	else
		input_args.scheduler = 3;
	return (input_args);
}

int	main(int argc, char **argv)
{
	input_args	input_args;

	if (argc != 9)
		return (fprintf(stderr, "Wrong input\n"), 1);
	input_args = parse_arguments(argv);
	if (input_args.scheduler == 3)
		return (fprintf (stderr, "Scheduler doesnt exist\n"), 1);
	print_args_struct(input_args);
}
