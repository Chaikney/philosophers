#include "philo.h"

// TODO Add 42 header.

// Log state changes of a philosopher.
// FIXME The way time works here we could report out of order and late without knowing.
// TODO Move to a struct for reporting: philosopher, state, timestamp (compare to report time)
void	report_state(t_plato phil, int state)
{
	struct timeval	now;
	u_int64_t		milli;
	int	seat;

	pthread_mutex_lock(&phil.data->report);
	seat = phil.seat;
	if ((state > 0) && (state < 6))
	{
		gettimeofday(&now, NULL);
		milli = ms_after(now, phil.data->started);
		printf("%lu %i ", milli, seat);
		if (state == HAS)
			printf("has taken a fork\n");
		if (state == EAT)
			printf("is eating\n");
		if (state == NAP)
			printf("is sleeping\n");
		if (state == HMM)
			printf("is thinking\n");
		if (state == DIE)
			printf("died\n");
	}
	pthread_mutex_unlock(&phil.data->report);
}

// Alternate report_state using a struct of all needed data
void	log_action(t_plato p, t_logmsg msg)
{
	struct timeval	now;
	u_int64_t		milli;

	pthread_mutex_lock(&p.data->report);
	if ((msg.state > 0) && (msg.state < 6))
	{
		gettimeofday(&now, NULL);
		milli = ms_after(msg.event_time, p.data->started);
		printf("%lu %i ", milli, p.seat);
		if (msg.state == HAS)
			printf("has taken a fork\n");
		if (msg.state == EAT)
			printf("is eating\n");
		if (msg.state == NAP)
			printf("is sleeping\n");
		if (msg.state == HMM)
			printf("is thinking\n");
		if (msg.state == DIE)
			printf("died\n");
	}
	pthread_mutex_unlock(&p.data->report);
}

// I guess this has to be a loop that breaks when the meal condition is met.
// TODO Add checks for the ability to grab a fork and a pause or release when it fails
// Remember that each of these threads is independent but trying to access shared things,.
// Maybe first imagine the philosoper as individualists
// TODO Do I have to run lock a philosopher's record as well (what does that mean?)
// TODO Need to check to see if ANY philo has died - implies storing in data
// TODO Give this a better name than launch_phil
// FIXME Wild. Someone dies at 30 ms no matter what the parameters.
void	launch_phil(void *ptr)
{
	t_plato	p;
	struct timeval	now;

	p = (*((t_plato *) ptr));	// NOTE all these brackets, Cast to t_plato first, then deref.
//	print_placecard(p);	// HACK for debugging
//	TODO Add a break-from loop condition for dead philosophers
	while ((p.eaten < p.data->appetite) || (p.data->appetite = -1))
	{
		take_forks(p);
		eat_food(p);
		replace_forks_and_nap(p);
		report_state(p, HMM);
		gettimeofday(&now, NULL);
		// FIXME Something is wrong with the death condition.
		if (ms_after(now, p.starve_at) > 0)	// HACK This is not readable or logical
		{
			// NOTE A return from this function is equivalent to ending the thread
			// (Can't use pthread_exit)
			report_state(p, DIE);
			return ;
//			exit(EXIT_SUCCESS);	// TODO End simulation routine needed
		}
	}
}

// Get parameters stored
// Create an array of mutexes to represent forks
// Set up philosoper characteristics
// link mutexes to philosophers
// Launch a thread for each philosopher
// TODO Try compilation without thread sanitize option
// TODO Function too long, some set up to move elsewhere.
// FIXME Is reporting off by a factor of 10?
int	main(int argc, char **argv)
{
	t_plato		*philo;
	t_table	*house_rules;
	pthread_mutex_t	*forks;
	int	i;

	i = 0;
	house_rules = malloc(sizeof(t_table));
	get_general_data(house_rules, argc, argv);
	print_menu(*house_rules);
	philo = NULL;
	philo = malloc(sizeof(t_plato) * house_rules->table_size);
	setup_philos(philo, house_rules);
	forks = malloc(sizeof(pthread_mutex_t) * house_rules->table_size);
	forks_laid(forks, philo);
	while (i < (house_rules->table_size) - 1)
	{
		pthread_create(&philo[i].id, NULL, (void *) &launch_phil, &philo[i]);
		i++;
	}
	// FIXME This intends to wait for all threads to join; don't think it does that.
	i = 0;
	while (i++ < (house_rules->table_size - 1))
		pthread_join(philo[i].id, NULL);
//	usleep(10000000);
	free(forks);
	free(house_rules);
	free(philo);
	printf("änd now i finish");
	return (0);
}
