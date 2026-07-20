/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 13:52:05 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/20 17:10:58 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_message(t_arguments *args, int coder_id, char *message)
{
	long	elapsed_ms;

	pthread_mutex_lock(&args->log_lock);
	elapsed_ms = get_ms_time() - args->start_time;
	printf("%ld %d %s\n", elapsed_ms, coder_id, message);
	pthread_mutex_unlock(&args->log_lock);
}

bool	cooldown_elapsed(t_arguments *args, t_dongle *dongle)
{
	if (dongle->released_at == -1)
		return (true);
	if (dongle->released_at + args->dongle_cooldown < get_ms_time())
		return (true);
	return (false);
}

void	register_waiter(t_coder *coder, t_dongle *dongle)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		if (dongle->queue[i].active == false)
		{
			dongle->queue[i].coder_id = coder->id;
			dongle->queue[i].arrival_time = get_ms_time();
			dongle->queue[i].deadline = coder->last_compile_start
				+ coder->arguments->time_to_burnout;
			dongle->queue[i].active = true;
			return ;
		}
		i++;
	}
}

void	unregister_waiter(t_dongle *dongle, int coder_id)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		if (dongle->queue[i].coder_id == coder_id)
			dongle->queue[i].active = false;
		i++;
	}
}


int	get_next_coder_id(t_arguments *args, t_dongle *dongle)
{
	int		i;
	int		best_id;
	long	best_key;
	long	current_key;

	i = 0;
	best_key = -1;
	while (i < 2)
	{
		if (dongle->queue[i].active == true)
		{
			if (args->scheduler == FIFO)
				current_key = dongle->queue[i].arrival_time;
			else
				current_key = dongle->queue[i].deadline;
			if (best_key == -1 || current_key < best_key)
			{
				best_key = current_key;
				best_id = dongle->queue[i].coder_id;
			}
		}
		i++;
	}
	return (best_id);
}


void	helper_pick_up_dongle(t_coder *coder, t_dongle *dongle)
{
	t_arguments *args;

	args = coder->arguments;
	pthread_mutex_lock(&dongle->lock);
	register_waiter(coder, dongle);
	while(dongle->in_use
		|| !cooldown_elapsed(coder->arguments, dongle)
		|| get_next_coder_id(args, dongle) != coder->id)
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	unregister_waiter(dongle, coder->id);
	dongle->in_use = 1;
	pthread_mutex_unlock(&dongle->lock);
	log_message(coder->arguments, coder->id, "has taken a dongle");
}

void	pick_up_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	if (coder->id % 2 == 0)
	{
		first = coder->left;
		second = coder->right;
	}
	else
	{
		first = coder->right;
		second = coder->left;
	}
	helper_pick_up_dongle(coder, first);
	if (first == second)
		return ;
	helper_pick_up_dongle(coder, second);
}


void	*coder_routine(void *arg)
{
	t_coder		*coder;
	t_arguments	*arguments;

	coder = (t_coder *) arg;
	arguments = coder->arguments;
	pick_up_dongle(coder);
	return NULL;
}

int	main(int argc, char **argv)
{
	t_arguments	args;

	pthread_mutex_init(&args.log_lock, NULL);
	pthread_mutex_init(&args.stop_lock, NULL);
	if (argc != 9)
		return (fprintf(stderr, "Wrong input\n"), 1);
	args = parse_arguments(argv);
	if (check_arguments(args) == -1)
		return (1);
	if (init_dongles(&args) == -1)
		return (1);
	if (init_coders(&args) == -1)
		return (1);
	distribute_dongles(&args);
	print_args_struct(args);
	int	i;
	i = 0;
	while (i < args.number_of_coders)
	{
	    if (pthread_create(&args.coders[i].thread, NULL, coder_routine, &args.coders[i]) != 0)
	        return (1);
	    i++;
	}
	i = 0;
	while (i < args.number_of_coders)
	{
	    pthread_join(args.coders[i].thread, NULL);
	    i++;
	}
	return (0);
}
