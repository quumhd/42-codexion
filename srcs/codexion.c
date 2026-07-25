/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:10:23 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/25 12:55:09 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#define  A1 "Arguments required: "
#define  A2 "<number_of_coders> <time_to_burnout> "
#define  A3 "<time_to_compile> <time_to_debug "
#define  A4 "<time_to_refactor <number_of_compile_required> "
#define  A5 "<dongle_cooldown> <scheduler>\n"

void	join_coders(t_arguments *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_join(args->coders[i].thread, NULL);
		i++;
	}
	pthread_join(args->wake_up_thread, NULL);
	pthread_join(args->monitoring_thread, NULL);
}

void	free_all(t_arguments *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_mutex_destroy(&args->coders[i].lock);
		pthread_mutex_destroy(&args->dongles[i].lock);
		pthread_cond_destroy(&args->dongles[i].cond);
		i++;
	}
	pthread_mutex_destroy(&args->stop_lock);
	pthread_mutex_destroy(&args->log_lock);
	free(args->coders);
	free(args->dongles);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_arguments	*arguments;

	coder = (t_coder *) arg;
	arguments = coder->arguments;
	while (has_to_stop(arguments) == false)
	{
		if (has_to_stop(arguments) == false)
			pick_up_dongle(coder);
		if (has_to_stop(arguments) == false)
			coder_compile(coder);
		if (has_to_stop(arguments) == false)
			put_dongles_down(coder);
		if (has_to_stop(arguments) == false)
			coder_debugg(coder);
		if (has_to_stop(arguments) == false)
			coder_refactor(coder);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_arguments	args;

	if (argc != 9)
		return (fprintf(stderr, "%s%s%s%s%s", A1, A2, A3, A4, A5), 1);
	args = parse_arguments(argv);
	pthread_mutex_init(&args.log_lock, NULL);
	pthread_mutex_init(&args.stop_lock, NULL);
	args.stop = false;
	if (check_arguments(args) == -1
		|| init_dongles(&args) == -1
		|| init_coders(&args) == -1)
		return (1);
	distribute_dongles(&args);
	if (start_monitoring(&args) == -1
		|| start_coders(&args) == -1)
		return (free_all(&args),1);
	join_coders(&args);
	free_all(&args);
	return (0);
}
