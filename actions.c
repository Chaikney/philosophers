#include "philo.h"

// TODO Add 42 header

// NOTE It seems very likely that we can be stuck in this function a while.
// ...what then to do with catching death conidtiions within 10ms?
// TODO Avoid deadlock by dropping left fork if can't get right one?
// But if you can't get the lock what happens? Wait at this point, or fail?
// NOTE The locks taken here are released in replace_forks_and_nap
void	take_forks(t_plato p)
{
	pthread_mutex_lock(p.l_fork);
	log_action(p, make_msg(HAS, p.seat));
	pthread_mutex_lock(p.r_fork);
	log_action(p, make_msg(HAS, p.seat));
}

// Report eating, update starvation time, increment meal count
// sleep for eat_time before returning
// Clearly this can't be safely called without p holding locks
// TODO Should this take a pointer to p, as we alter the starve_at time.
void	eat_food(t_plato p)
{
	struct timeval	now;

	log_action(p, make_msg(EAT, p.seat));
	gettimeofday(&now, NULL);
	p.starve_at = add_ms(now, p.data->die_time);
	p.eaten++;	// TODO Do these records need to be locked while updating?
	if ((p.is_sated == 0) && (p.eaten >= p.data->appetite))
	{
		pthread_mutex_lock(&p.data->update);
		p.data->sated++;
		p.is_sated = 1;
		pthread_mutex_unlock(&p.data->update);
	}
	usleep(p.data->eat_time * 1000);
}

// Release the forks held and have a nap
void	replace_forks_and_nap(t_plato p)
{
	pthread_mutex_unlock(p.l_fork);
	pthread_mutex_unlock(p.r_fork);
	log_action(p, make_msg(NAP, p.seat));
	usleep(p.data->nap_time * 1000);
}

// Check to see if the philosopher has gone beyond starving time.
// If yes, report message, decrease number of living at table.
// TODO Should I flag the philo as being dead so can end loop?
void	take_pulse(t_plato p)
{
	struct timeval	now;

	gettimeofday(&now, NULL);
	if (ms_after(now, p.starve_at))
	{
		log_action(p, make_msg(DIE, p.seat));
		pthread_mutex_lock(&p.data->update);
		p.data->living--;
		p.is_dead = 1;
		pthread_mutex_unlock(&p.data->update);
	}
}
