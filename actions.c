/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 09:42:43 by chaikney          #+#    #+#             */
/*   Updated: 2024/06/24 09:42:48 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// NOTE It seems very likely that we can be stuck in this function a while.
// ...what then to do with catching death conidtiions within 10ms?
// TODO Avoid deadlock by dropping left fork if can't get right one?
// But if you can't get the lock what happens? Wait at this point, or fail?
// NOTE The locks taken here are released in replace_forks_and_nap
// NOTE This is more noticable *without* valgrind!
void	take_forks(t_plato *p)
{
	if ((p->is_dead == 0) && (p->data->stop == 0))
	{
		if (p->is_sated == 1)
			usleep(p->data->die_time / 10 / 1000);
		if (p->seat % 2 == 0)
			pthread_mutex_lock(p->l_fork);
		else
			pthread_mutex_lock(p->r_fork);
		log_action(*p, make_msg(HAS, p->seat));
		take_pulse(p);
		if (p->is_dead == 0)
		{
			if (p->seat % 2 == 0)
				pthread_mutex_lock(p->r_fork);
			else
				pthread_mutex_lock(p->l_fork);
			log_action(*p, make_msg(HAS, p->seat));
		}
		else
			pthread_mutex_unlock(p->l_fork);
	}
	else
		pthread_mutex_unlock(p->l_fork);
}

// Report eating, update starvation time, increment meal count
// sleep for eat_time before returning
// Clearly this can't be safely called without p holding locks
void	eat_food(t_plato *p)
{
	struct timeval	now;

	if ((p->is_dead == 0) && (p->data->stop == 0))
	{
		log_action(*p, make_msg(EAT, p->seat));
		gettimeofday(&now, NULL);
		p->starve_at = add_ms(now, p->data->die_time);
		p->eaten++;
		if ((p->is_sated == 0) && (p->eaten >= p->data->appetite))
		{
			pthread_mutex_lock(&p->data->update);
			p->data->sated++;
			p->is_sated = 1;
			if (all_done(p) == 1)
				p->data->stop = 1;
			pthread_mutex_unlock(&p->data->update);
		}
		usleep(p->data->eat_time * 1000);
	}
}

// Release the forks held and have a nap
void	replace_forks_and_nap(t_plato p)
{
	pthread_mutex_unlock(p.l_fork);
	pthread_mutex_unlock(p.r_fork);
	if ((p.is_dead == 0) && (p.data->stop == 0))
	{
		log_action(p, make_msg(NAP, p.seat));
		usleep(p.data->nap_time * 1000);
	}
}
