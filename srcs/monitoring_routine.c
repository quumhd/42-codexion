/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring_routine.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:11:53 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/23 13:25:18 by jdreissi         ###   ########.fr       */
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
	int			i;
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

bool	burnout(t_arguments *arguments)
{
	int		i;
	t_coder	*coders;
	long	time_since_last_compile;

	i = 0;
	coders = arguments->coders;
	time_since_last_compile = 0;
	pthread_mutex_lock(&arguments->coders[i].lock);
	time_since_last_compile = get_ms_time() - coders[i].last_compile_start;
	pthread_mutex_unlock(&arguments->coders[i].lock);
	while (i < arguments->number_of_coders)
	{
		if (time_since_last_compile > arguments->time_to_burnout)
		{
			log_message(arguments, coders[i].id, "has burned out");
			pthread_mutex_lock(&arguments->stop_lock);
			arguments->stop = true;
			wake_up_all(arguments);
			pthread_mutex_unlock(&arguments->stop_lock);
			return (true);
		}
		i++;
	}
	return (false);
}

bool	reached_compile_goal(t_arguments *arguments, t_coder *coders)
{
	int	i;
	int	compiles_required;
	int	finished_compiling;

	i = 0;
	compiles_required = arguments->number_of_compiles_required;
	finished_compiling = 0;
	while (i < arguments->number_of_coders)
	{
		pthread_mutex_lock(&arguments->coders[i].lock);
		if (coders[i].number_of_finished_compiles >= compiles_required)
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
		return (true);
	}
	return (false);
}

void	*monitoring_routine(void *args)
{
	t_coder		*coders;
	t_arguments	*arguments;

	arguments = (t_arguments *) args;
	coders = arguments->coders;
	while (true)
	{
		if (burnout(arguments) == true)
			return (NULL);
		if (reached_compile_goal(arguments, coders) == true)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
