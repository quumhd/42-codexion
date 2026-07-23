/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:10:23 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/23 14:24:59 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_monitoring(t_arguments *args)
{
	pthread_t	wake_up_thread;
	pthread_t	monitoring_thread;

	if (pthread_create(&wake_up_thread, NULL, wake_up_routine, args))
		return (fprintf(stderr, "Failed creating thread\n"), -1);
	if (pthread_create(&monitoring_thread, NULL, monitoring_routine, args))
		return (fprintf(stderr, "Failed creating thread\n"), -1);
	return (0);
}

int	start_coders(t_arguments *args)
{
	int		i;
	t_coder	*coders;

	i = 0;
	coders = args->coders;
	while (i < args->number_of_coders)
	{
		if (pthread_create(&coders[i].thread, NULL, coder_routine, &coders[i]))
			return (fprintf(stderr, "Failed creating thread\n"), -1);
		i++;
	}
	return (0);
}

void	join_coders(t_arguments *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_join(args->coders[i].thread, NULL);
		i++;
	}
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_arguments	*arguments;

	coder = (t_coder *) arg;
	arguments = coder->arguments;
	while (has_to_stop(arguments) == false)
	{
		pick_up_dongle(coder);
		if (has_to_stop(arguments) == true)
			break ;
		coder_compile(coder);
		put_dongles_down(coder);
		coder_debugg(coder);
		coder_refactor(coder);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_arguments	args;

	if (argc != 9)
		return (fprintf(stderr, "Arguments required: "
				"<number_of_coders> <time_to_burnout> "
				"<time_to_compile> <time_to_debug "
				"<time_to_refactor <number_of_compile_required> "
				"<dongle_cooldown> <scheduler>\n"), 1);
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
		return (1);
	join_coders(&args);
	free (args.coders);
	free (args.dongles);
	return (0);
}
