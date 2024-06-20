#include "philo.h"

// TODO Add 42 header

// NOTE It seems very likely that we can be stuck in this function a while.
// ...what then to do with catching death conidtiions within 10ms?
// TODO Avoid deadlock by dropping left fork if can't get right one?
// But if you can't get the lock what happens? Wait at this point, or fail?
void	take_forks(t_plato p)
{
	pthread_mutex_lock(p.l_fork);
	report_state(p, HAS);
	pthread_mutex_lock(p.r_fork);
	report_state(p, HAS);
}

// Report eating, update starvation time, increment meal count
// sleep for eat_time before returning
// Clearly this can't be safely called without p holding locks
void	eat_food(t_plato p)
{
	struct timeval	now;

	report_state(p, EAT);
	gettimeofday(&now, NULL);
	p.starve_at = add_ms(now, p.data->die_time);
	p.eaten++;	// NOTE Does this record need to be locked while updating?
	usleep(p.data->eat_time * 1000);
}

// Release the forks held and have a nap
void	replace_forks_and_nap(t_plato p)
{
	pthread_mutex_unlock(p.l_fork);
	pthread_mutex_unlock(p.r_fork);
	report_state(p, NAP);
	usleep(p.data->nap_time * 1000);
}
