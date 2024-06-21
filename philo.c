#include "philo.h"

// TODO Add 42 header.

// Log state changes of a philosopher.
// FIXED The way time works here we could report out of order and late without knowing.
// DONE Move to a struct for reporting: philosopher, state, timestamp (compare to report time)
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
// Lock access to this so that no other thread prints something in the middle
// DONE Every place that there is report_state we have this instead
// DONE Implement something to make logmsg (timestamp especially)
void	log_action(t_plato p, t_logmsg *msg)
{
	struct timeval	now;
	u_int64_t		milli;
	u_int64_t		delay;

	pthread_mutex_lock(&p.data->report);
	if ((msg->state > 0) && (msg->state < 6))
	{
		gettimeofday(&now, NULL);
		milli = ms_after(msg->event_time, p.data->started);
		delay = ms_after(now, msg->event_time);
		printf("%lu %i ", milli, p.seat);
		printf("(delay: %lu)", delay);
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
		if (delay >= 10)
			printf("\n********ALERT!!!********* delayed message");
	}
	pthread_mutex_unlock(&p.data->report);
	free(msg);	// NOTE Best free the message as soon as it used.
}

//Return a log message to be used then freed
t_logmsg	*make_msg(int state, int seat)
{
	t_logmsg	*msg;
	struct timeval	now;

	msg = malloc(sizeof(t_logmsg));
	if (!msg)
		return(NULL);
	gettimeofday(&now, NULL);
	msg->philo = seat;
	msg->state = state;
	msg->event_time = now;
	return (msg);
}

// Return 1 if all the philosophers have eaten their fill
int	all_done(t_plato p)
{
	int	ans;

	ans = 0;
	pthread_mutex_lock(&p.data->report);
	if (p.data-> appetite != -1)
	{
		if (p.data->sated == p.data->table_size)
			ans = 1;
		if (p.data->sated > p.data->table_size)
		{
			printf("\n******** BAD COUNT ******************\n");
			exit(EXIT_FAILURE);	// NOTE If this happens there is a problem in eat action.
		}
	}
	pthread_mutex_unlock(&p.data->report);
	return (ans);
}

// Return 1 if *any* philosopher has died
int	call_ambulance(t_plato p)
{
	int	ans;

	ans = 0;
	pthread_mutex_lock(&p.data->report);
	if (p.data->living < p.data->table_size)
		ans = 1;
	pthread_mutex_unlock(&p.data->report);
	return (ans);
}

// I guess this has to be a loop that breaks when the meal condition is met.
// Remember that each of these threads is independent but trying to access shared things,.
// Maybe first imagine the philosoper as individualists
// TODO Do I have to run lock a philosopher's record as well (what does that mean?)
// DONE Some of these functions might need to take a pointer?
// FIXME Check for *any* dead philo should end the sim for *all threads*
// TODO Add a immediate finish function to the call_ambulance?
// FIXME Endless loop again.
void	dining_loop(void *ptr)
{
	t_plato	p;

	p = (*((t_plato *) ptr));	// NOTE all these brackets, Cast to t_plato first, then deref.
//	print_placecard(p);	// HACK for debugging
//	NOTE The thread ends when either the philosopher dies or everyone has eaten their fill
//	FIXME This does not work in the case of someone dying. Deadlocks remain somewhere
//	FIXME If all philos have eaten their fill, things continue.
	while ((p.is_dead == 0) && (all_done(p) == 0) && (call_ambulance(p) == 0))
	{
		take_forks(p);
		eat_food(&p);
		replace_forks_and_nap(p);
		log_action(p, make_msg(HMM, p.seat));
		take_pulse(&p);
	}
	printf("philo %i has finished", p.seat);
}

// Destroy muitexes, free memory, etc to ensure that we finish the sim cleanly
void	clear_table(pthread_mutex_t *forks, t_plato *philos, t_table *rules)
{
	int	n;
	int	i;

	n = rules->table_size;
	i = 0;
	while (i < n)
		pthread_mutex_destroy(&forks[i]);
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
// TODO Function too long, some set up to move elsewhere.
// FIXME Is reporting off by a factor of 10?
// TODO Perhaps restructure the setup functions to return values (saves space here)
int	main(int argc, char **argv)
{
	t_plato		*philo;
	t_table	*house_rules;
	pthread_mutex_t	*forks;
	int	i;

	i = 0;
	house_rules = malloc(sizeof(t_table));
	get_general_data(house_rules, argc, argv);
	print_menu(*house_rules);	// HACK for debugging, remove later
	philo = NULL;
	philo = malloc(sizeof(t_plato) * house_rules->table_size);
	setup_philos(philo, house_rules);
	forks = malloc(sizeof(pthread_mutex_t) * house_rules->table_size);
	forks_laid(forks, philo);
	while (i < (house_rules->table_size) - 1)
	{
		pthread_create(&philo[i].id, NULL, (void *) &dining_loop, &philo[i]);
		i++;
	}
	// FIXME This intends to wait for all threads to join; don't think it does that.
	i = 0;
	while (i++ < (house_rules->table_size - 1))
		pthread_join(philo[i].id, NULL);
	clear_table(forks, philo, house_rules);
	printf("änd now i finish");
	return (0);
}
