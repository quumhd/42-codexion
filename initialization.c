/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialization.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:16:44 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/20 16:29:30 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


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
		args->dongles[dongle_id].queue[0].active = false;
		args->dongles[dongle_id].queue[1].active = false;
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
