/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring_threads.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:11:53 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/22 17:20:59 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wake_up_all(t_arguments *args)
{
	int	i;

	i = 0;
	while (i < args->number_of_coders)
	{
		pthread_mutex_lock(&args->dongles[i].lock);
		pthread_cond_broadcast(&args->dongles[i].cond);
		pthread_mutex_unlock(&args->dongles[i].lock);
		i++;
	}
}

void	*wake_up_routine(void *args)
{
	int         i;
	t_arguments	*arguments;

	i = 0;
	arguments = (t_arguments *) args;
	while (has_to_stop(arguments) == false)
	{
		while (i < arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->dongles[i].lock);
			if (arguments->dongles[i].in_use == false)
			{
				if (cooldown_elapsed(arguments, &arguments->dongles[i]) == true)
					pthread_cond_broadcast(&arguments->dongles[i].cond);
			}
			pthread_mutex_unlock(&arguments->dongles[i].lock);
			i++;
		}
		usleep(1000);
		i = 0;
	}
	return (NULL);
}

void	*monitoring_routine(void *args)
{
	int			i;
	t_coder		*coders;
	t_arguments	*arguments;
	int			finished_compiling;
	long		time_since_last_compile;

	i = 0;
	finished_compiling = 0;
	arguments = (t_arguments *) args;
	coders = arguments->coders;
	time_since_last_compile = 0;
	while (true)
	{
		while (i < arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->coders[i].lock);
			time_since_last_compile = get_ms_time() - coders[i].last_compile_start;
			pthread_mutex_unlock(&arguments->coders[i].lock);
			if (time_since_last_compile > arguments->time_to_burnout)
			{
				log_message(arguments, coders[i].id, "has burned out");
				pthread_mutex_lock(&arguments->stop_lock);
				arguments->stop = true;
				wake_up_all(arguments);
				pthread_mutex_unlock(&arguments->stop_lock);
				return (NULL);
			}
			pthread_mutex_lock(&arguments->coders[i].lock);
			if (coders[i].number_of_finished_compiles >= arguments->number_of_compiles_required)
				finished_compiling++;
			pthread_mutex_unlock(&arguments->coders[i].lock);
			i++;
		}
		if (finished_compiling >= arguments->number_of_coders)
		{
			pthread_mutex_lock(&arguments->stop_lock);
			arguments->stop = true;
			wake_up_all(arguments);
			pthread_mutex_unlock(&arguments->stop_lock);
			return (NULL);
		}
		finished_compiling = 0;
		usleep(1000);
		i = 0;
	}
	return (NULL);
}
