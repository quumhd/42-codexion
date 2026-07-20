/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_time.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdreissi <jdreissi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 15:00:42 by jdreissi          #+#    #+#             */
/*   Updated: 2026/07/20 15:00:43 by jdreissi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


long get_ms_time()
{
    long    time_in_ms;
    struct  timeval tv;
    gettimeofday(&tv, NULL);
    time_in_ms = tv.tv_sec * 1000L + tv.tv_usec / 1000L;
    return (time_in_ms);
}