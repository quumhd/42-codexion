/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   select_order.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:22:20 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/22 17:22:41 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		if (dongle->queue[i].active && dongle->queue[i].coder_id == coder_id)
		{
			dongle->queue[i].active = false;
			return ;
		}
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
	best_id = -1;
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
