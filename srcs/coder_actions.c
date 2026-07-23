/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/22 17:14:27 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/23 12:41:08 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
	helper_pick_up_dongle(coder, second);
}

void	put_dongles_down(t_coder *coder)
{
	pthread_mutex_lock(&coder->left->lock);
	coder->left->in_use = false;
	coder->left->released_at = get_ms_time();
	pthread_cond_broadcast(&coder->left->cond);
	pthread_mutex_unlock(&coder->left->lock);
	pthread_mutex_lock(&coder->right->lock);
	coder->right->in_use = false;
	coder->right->released_at = get_ms_time();
	pthread_cond_broadcast(&coder->right->cond);
	pthread_mutex_unlock(&coder->right->lock);
}

void	coder_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_start = get_ms_time();
	pthread_mutex_unlock(&coder->lock);
	log_message(coder->arguments, coder->id, "is compiling");
	usleep(coder->arguments->time_to_compile * 1000);
	pthread_mutex_lock(&coder->lock);
	coder->number_of_finished_compiles++;
	pthread_mutex_unlock(&coder->lock);
}

void	coder_debugg(t_coder *coder)
{
	log_message(coder->arguments, coder->id, "is debugging");
	usleep(coder->arguments->time_to_debug * 1000);
}

void	coder_refactor(t_coder *coder)
{
	log_message(coder->arguments, coder->id, "is refactoring");
	usleep(coder->arguments->time_to_refactor * 1000);
}
