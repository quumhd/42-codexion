/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:10:59 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/23 12:48:27 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_ms_time(void)
{
	long			time_in_ms;
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	time_in_ms = tv.tv_sec * 1000L + tv.tv_usec / 1000L;
	return (time_in_ms);
}

bool	has_to_stop(t_arguments *args)
{
	bool	stop;

	pthread_mutex_lock(&args->stop_lock);
	stop = args->stop;
	pthread_mutex_unlock(&args->stop_lock);
	return (stop);
}

bool	cooldown_elapsed(t_arguments *args, t_dongle *dongle)
{
	if (dongle->released_at == -1)
		return (true);
	if (dongle->released_at + args->dongle_cooldown < get_ms_time())
		return (true);
	return (false);
}

void	helper_pick_up_dongle(t_coder *coder, t_dongle *dongle)
{
	t_arguments	*args;

	args = coder->arguments;
	pthread_mutex_lock(&dongle->lock);
	register_waiter(coder, dongle);
	while ((dongle->in_use
			|| !cooldown_elapsed(coder->arguments, dongle)
			|| get_next_coder_id(args, dongle) != coder->id)
		&& has_to_stop(args) == false)
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	unregister_waiter(dongle, coder->id);
	dongle->in_use = true;
	pthread_mutex_unlock(&dongle->lock);
	if (has_to_stop(args) == true)
		return ;
	log_message(coder->arguments, coder->id, "has taken a dongle");
}

void	log_message(t_arguments *args, int coder_id, char *message)
{
	long	elapsed_ms;

	pthread_mutex_lock(&args->log_lock);
	elapsed_ms = get_ms_time() - args->start_time;
	printf("%ld %d %s\n", elapsed_ms, coder_id, message);
	pthread_mutex_unlock(&args->log_lock);
}
