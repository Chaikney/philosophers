#include "philo.h"

// TODO Add 42 header.

// Alternate report_state using a struct of all needed data
// Lock access to this so that no other thread prints something in the middle
void	log_action(t_plato p, t_logmsg *msg)
{
	struct timeval	now;
	u_int64_t		milli;

	pthread_mutex_lock(&p.data->report);
	if (p.data->stop == 0)
	{
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
	}
	pthread_mutex_unlock(&p.data->report);
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
// FIXED exit is a forbidden function, remove it.
// TODO Move this to a check done after a philo eats.
// TODO Have this set t_table->stop
int	all_done(t_plato *p)
{
	int	ans;

	ans = 0;
	pthread_mutex_lock(&p->data->report);
	if (p->data-> appetite != -1)
	{
		if (p->data->sated == p->data->table_size)
			ans = 1;
		if (p->data->sated > p->data->table_size)
			printf("\n******** BAD COUNT ******************\n");
	}
	pthread_mutex_unlock(&p->data->report);
	return (ans);
}

// Return 1 if *any* philosopher has died
// TODO Too complicated, replace with an end simulation flag.(Replace func entirely?)
int	call_ambulance(t_plato p)
{
	int	ans;

	ans = 0;
	pthread_mutex_lock(&p.data->report);
	if (p.data->stop == 1)
		ans = 1;
	pthread_mutex_unlock(&p.data->report);
	return (ans);
}

// I guess this has to be a loop that breaks when the meal condition is met.
// Remember that each thread is independent but trying to access shared things.
// Maybe first imagine the philosoper as individualists
// TODO Do I have to run lock a philosopher's record as well (what does that mean?)
// DONE Some of these functions might need to take a pointer?
// FIXED Check for *any* dead philo should end the sim for *all threads*
// TODO Add a immediate finish function to the call_ambulance?
// FIXED Endless loop again. -- caused by deadlock I think.
// NOTE all these brackets, Cast to t_plato first, then deref.
//	One dead philo = stop immediately
//	Reach the number of meals for all == stop immediately as well.
//	Currently they continue and only check at the end - problem if stuck waiting for forks
//	FIXME If all philos have eaten their fill, things continue.
void	dining_loop(void *ptr)
{
	t_plato	p;

	p = (*((t_plato *) ptr));
//	print_placecard(p);	// HACK for debugging
	while (p.data->stop == 0)
	{
		take_forks(&p);
		eat_food(&p);
		replace_forks_and_nap(p);
		log_action(p, make_msg(HMM, p.seat));
		take_pulse(&p);
	}
}

// Destroy mutexes, free memory, etc to ensure that we finish the sim cleanly
void	clear_table(pthread_mutex_t *forks, t_plato *philos, t_table *rules)
{
	int	n;
	int	i;

	n = rules->table_size;
	i = 0;
	while (i < n)
		pthread_mutex_destroy(&forks[i++]);
	pthread_mutex_destroy(&rules->report);
	pthread_mutex_destroy(&rules->update);
	free(forks);
	free(rules);
	free(philos);
}

// Get parameters stored
// Create an array of mutexes to represent forks
// Set up philosoper characteristics
// link mutexes to philosophers
// Launch a thread for each philosopher
// TODO Try compilation without thread sanitize option
// FIXME Is reporting off by a factor of 10?
// FIXED Main problem now is immediate halt one one philo dies
// DONE Perhaps restructure the setup functions to return values (saves space here)
int	main(int argc, char **argv)
{
	t_plato			*philo;
	t_table			*house_rules;
	pthread_mutex_t	*forks;
	int				i;

	if (check_data(argc, argv) == 0)
	{
		i = 1;
		house_rules = get_general_data(argc, argv);
//	print_menu(*house_rules);	// HACK for debugging, remove later
		philo = setup_philos(house_rules);
		forks = forks_laid(philo,  house_rules->table_size);
		while (i <= (house_rules->table_size))
		{
			pthread_create(&philo[i - 1].id, NULL, (void *) &dining_loop, &philo[i - 1]);
			i++;
		}
		i = 0;
		while (i++ < (house_rules->table_size) -1)
			pthread_join(philo[i].id, NULL);
		clear_table(forks, philo, house_rules);
	}
	printf("änd now i finish");
	return (0);
}
