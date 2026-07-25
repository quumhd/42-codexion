/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manage_threads.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:34:08 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/25 12:25:46 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	start_monitoring(t_arguments *args)
{
	if (pthread_create(&args->wake_up_thread, NULL, wake_up_routine, args))
		return (fprintf(stderr, "Failed creating thread\n"), -1);
	if (pthread_create(&args->monitoring_thread, NULL, monitoring_routine, args))
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
