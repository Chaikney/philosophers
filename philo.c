#include "philo.h"

// TODO Add 42 header.

// handle the parameters, must only  be positive ints
// Return -1 is the input is invalid.
int	ph_atoi(char *str)
{
	int	n;
	int	d;

	n = 0;
	while (*str == 32 || (*str <= 13 && *str >= 9))
		str++;
	if (*str == '-')
		return (-1);
	while (*str != '\0')
	{
		n = n * 10;
		if ((*str >= '0') && (*str <= '9'))
			d = *str - '0';
		else
			return (-1);
		str++;
		n = n + d;
	}
	return (n);
}

// Log state changes of a philosopher.
// TODO The timestamp should  not be "now" but "milliseconds since start"
void	report_state(int phil, int state)
{
	struct timeval	now;
	int			milli;

	if ((state > 0) && (state < 6))
	{
		gettimeofday(&now, NULL);
		milli = timeval_to_ms(now);
		printf("%i %i ", milli, phil);
		if (state == 1)
			printf("has taken a fork\n");
		if (state == 2)
			printf("is eating\n");
		if (state == 3)
			printf("is sleeping\n");
		if (state == 4)
			printf("is thinking\n");
		if (state == 5)
			printf("died\n");
	}
}

// Convert a timeval struct into an int representing a number of
// milliseconds
int	timeval_to_ms(struct timeval t)
{
	int	milliseconds;

	milliseconds = 0;
	milliseconds += t.tv_sec * 1000;
	milliseconds += t.tv_usec / 1000;
	return (milliseconds);
}

// Convert the given times in milliseconds to the timeval struct
// which uses seconds and microseconds
struct timeval	ms_to_timeval(int t)
{
	struct timeval	ret_time;

	ret_time.tv_sec = t / 1000;
	ret_time.tv_usec = (t % 1000) * 1000;
	return (ret_time);
}

// Debug function to print the house rules
void	print_menu(t_table t)
{
	printf("A booking for %i philophers\ndie after: %i\neat for: %i\nsleep for: %i\neat until: %i",
			t.table_size, t.die_time, t.eat_time, t.nap_time, t.appetite);
}

//Debug function to print things about the philsopher
void	print_placecard(t_plato p)
{
	printf("\nSitting in place: %i", p.seat);
}

void	launch_phil(void *ptr)
{
	//t_plato	p;

	(void) ptr;
	printf("in the launch thread what will happen");
//	p = (*((t_plato *) ptr));	// NOTE all these brackets, Cast to t_plato first, then deref.
//	print_thesis(p);
}

// FIXED Segfualt due to invalid write (nap_time, but maybe more gen problem as the thing moves)
// NOTE Fix was to allocate sizeof(t_table) before arriving here
void	get_general_data(t_table *dat, int argc, char **argv)
{

	if ((argc == 5) || (argc == 6))
	{
		dat->table_size = ph_atoi(argv[1]);
		dat->die_time = (ph_atoi(argv[2]));
		dat->eat_time = (ph_atoi(argv[3]));
		dat->nap_time = (ph_atoi(argv[4]));
		if (argc == 6)
			dat->appetite = ph_atoi(argv[5]);
		else
			dat->appetite = -1;
	}
	else
		exit(EXIT_FAILURE);
}

//. Allocate mempory for the philosopher things.
//Really hard not to call this "set table" I should get a prize
// allocate what is needed for one philo struct
// FIXME This is *nonsense*
// TODO Special case set up for first and last philosopher's forks
void	setup_philos(t_plato **phil, int n, pthread_mutex_t **fork)
{
	int	i;

	i = 0;
//	phil->id = malloc(sizeof(pthread_t *));	// FIXME is this needed? It does not work.

	while (i < n)
	{
		phil[i]->seat = n;
		phil[i]->l_fork = fork[n];
		phil[i]->r_fork = fork[n + 1];
		i++;
	}
}

// Allocate an array of mutexes. Initialise them, too?
// Seems a bit much when I have NO IDEA what I am doing with the threads, even.
void	forks_laid(pthread_mutex_t **forks, int n)
{
	int	i;

	i = 0;
	forks = malloc(sizeof(pthread_mutex_t) * n);
	(void) i;
	while (i < n)
		pthread_mutex_init(forks[i++], NULL);
}

// DONE Convert time parameters to an appropriate format (struct?)
// FIXME Segfault hit by pthread_create - incvalid write, what is not set up??
// TODO Allocate space for data for characteristics
int	main(int argc, char **argv)
{
//	int	i;
	t_plato		*philo;
	t_table	*house_rules;
	pthread_mutex_t	**forks;

	house_rules = malloc(sizeof(t_table));
	get_general_data(house_rules, argc, argv);
	print_menu(*house_rules);
	philo = NULL;
	forks = NULL;
	forks_laid(forks, house_rules->table_size);
	setup_philos(&philo, house_rules->table_size, forks);
	/* phil = 0; */
	/* phil[i] = 0; */
	/* while (i <= num_philos) */
	/* 	{ */
	/* 		// TODO Confirm that this format creates treads with mstly-identical data and unique seat */
	/* 		philcharacter->seat = i; */
	/* 		// FIXED This segfaults, invalid write: not allocated space for phil[i] ? */
	/* 		(pthread_create(&phil[i], NULL, (void *) &launch_phil, (void *) &philcharacter) ); */
	/* 		i++; */
	/* 	} */
	/* 	// NOTE the join call waits for all the threads to arrive. Without this the program */
		// gets to the end before thread 2 finishes.
		// NOTE I don't know if this form would work.
//		pthread_join(*phil, NULL);
		printf("änd now i finish");
	return (0);
}
