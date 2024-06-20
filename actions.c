#include "philo.h"

// TODO Add 42 header

// NOTE It seems very likely that we can be stuck in this function a while.
// ...what then to do with catching death conidtiions within 10ms?
// TODO Avoid deadlock by dropping left fork if can't get right one?
// But if you can't get the lock what happens? Wait at this point, or fail?
void	take_forks(t_plato p)
{
	t_logmsg	*msg;

	pthread_mutex_lock(p.l_fork);
	msg = make_msg(HAS, p.seat);
	log_action(p, msg);
//	report_state(p, HAS);
	pthread_mutex_lock(p.r_fork);
	msg = make_msg(HAS, p.seat);
	log_action(p, msg);
//	report_state(p, HAS);
}

// Report eating, update starvation time, increment meal count
// sleep for eat_time before returning
// Clearly this can't be safely called without p holding locks
void	eat_food(t_plato p)
{
	struct timeval	now;
	t_logmsg	*msg;

	msg = make_msg(EAT, p.seat);
	log_action(p, msg);
//	report_state(p, EAT);
	gettimeofday(&now, NULL);
	p.starve_at = add_ms(now, p.data->die_time);
	p.eaten++;	// TODO Do these records need to be locked while updating?
	if (p.eaten >= p.data->appetite)
	{
		pthread_mutex_lock(&p.data->update);
		p.data->sated++;
		pthread_mutex_unlock(&p.data->update);
	}
	usleep(p.data->eat_time * 1000);
}

// Release the forks held and have a nap
void	replace_forks_and_nap(t_plato p)
{
	t_logmsg	*msg;

	pthread_mutex_unlock(p.l_fork);
	pthread_mutex_unlock(p.r_fork);
	msg = make_msg(NAP, p.seat);
	log_action(p, msg);
//	report_state(p, NAP);
	usleep(p.data->nap_time * 1000);
}
