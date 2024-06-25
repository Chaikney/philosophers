/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 09:43:04 by chaikney          #+#    #+#             */
/*   Updated: 2024/06/24 09:43:09 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// Functions for logging and monitoring the philosophers.

// Alternate report_state using a struct of all needed data
// Lock access to this so that no other thread prints something in the middle
void	log_action(t_plato p, t_logmsg *msg)
{
	struct timeval	now;
	u_int64_t		milli;

	if (getset_stop(&p, 0) == 0)	// FIXME This is implicated in a data race with eat_food and take forks
	{
		pthread_mutex_lock(&p.data->update);
		gettimeofday(&now, NULL);
		milli = ms_after(msg->event_time, p.data->started);
		printf("%lu %i ", milli, p.seat);
		if (msg->state == HAS)
			printf("has taken a fork\n");
		if (msg->state == EAT)
			printf("is eating\n");
		if (msg->state == NAP)
			printf("is sleeping\n");
		if (msg->state == HMM)
			printf("is thinking\n");
		if (msg->state == DIE)
			printf("died\n");
		pthread_mutex_unlock(&p.data->update);
	}
	free(msg);
}

// Return a log message to be used then freed
t_logmsg	*make_msg(int state, int seat)
{
	t_logmsg		*msg;
	struct timeval	now;

	msg = malloc(sizeof(t_logmsg));
	if (!msg)
		return (NULL);
	gettimeofday(&now, NULL);
	msg->philo = seat;
	msg->state = state;
	msg->event_time = now;
	return (msg);
}

// Return 1 if all the philosophers have eaten their fill
// NOTE This is called after a philo is sated and from there triggers stop
// TODO This should be a "get / set" type thing, no?
// flag 1 - update (increase sated value)
// flag 0 - return the value (what value? done or not done)
int	all_done(t_plato *p, int flag)
{
	int	ans;

	ans = 0;
	pthread_mutex_lock(&p->data->update);
	if (flag == 0)
	{
		if ((p->data->sated == p->data->table_size) && (p->data->appetite != -1))
			ans = 1;
	}
	else if (flag == 1)
	{
		p->data->sated++;
	}
	pthread_mutex_unlock(&p->data->update);
	return (ans);
}

// Check to see if the philosopher has gone beyond starving time.
// If yes, report message, decrease number of living at table.
// NOTE We check that they aren't already marked as dead.
void	take_pulse(t_plato *p)
{
	struct timeval	now;

	if (getset_stop (p, 0) == 0)	// FIXME Implicated in data race
	{
		gettimeofday(&now, NULL);
		if ((p->is_dead == 0) && (ms_after(now, p->starve_at)))
		{
			log_action(*p, make_msg(DIE, p->seat));
			pthread_mutex_lock(&p->data->update);	// FIXME Should this be a Setter?
			p->data->stop = 1;
			p->is_dead = 1;
			pthread_mutex_unlock(&p->data->update);
		}
	}
}
