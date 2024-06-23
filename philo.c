#include "philo.h"

// TODO Add 42 header.

// I guess this has to be a loop that breaks when the meal condition is met.
// Remember that each thread is independent but trying to access shared things.
// Maybe first imagine the philosoper as individualists
// TODO Do I have to run lock a philosopher's record as well (what does that mean?)
// NOTE all these brackets, Cast to t_plato first, then deref.
//	One dead philo = stop immediately
//	Reach the number of meals for all == stop immediately as well.
//	Currently they continue and only check at the end - problem if stuck waiting for forks
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
