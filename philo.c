#include "philo.h"
#include <pthread.h>
#include <unistd.h>

// TODO Add 42 header.

// Log state changes of a philosopher.
// FIXED This is often overlapped, how to prevent that? Another mutex?
// ...that would mean only one thread accessing this code at once, so I guess yes.
// FIXED The timestamp does not work (with the mutex?) I get 0 always
// FIXED The report lock is not initialised?
void	report_state(t_plato phil, int state)
{
	struct timeval	now;
	int			milli;
	int	seat;

	pthread_mutex_lock(&phil.data->report);
	seat = phil.seat;
	if ((state > 0) && (state < 6))
	{
		gettimeofday(&now, NULL);
		milli = timeval_to_ms(now) - timeval_to_ms((phil.data->started));
		printf("%i %i ", milli, seat);
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
	pthread_mutex_unlock(&phil.data->report);	// FIXME Uninitialised value here
}

// Return 1 if the first time is later than deadline
// -1 if not
// 0 if equal (to millisecond precision(?))
// First compare seconds then if equal look at the microseconds
// TODO Could be a generalised calculating function instead?
int	is_later(struct timeval now, struct timeval deadline)
{
	if (now.tv_sec > deadline.tv_sec)
		return (1);
	if (now.tv_sec < deadline.tv_sec)
		return (-1);
	if ((now.tv_usec / 1000) > (deadline.tv_usec / 1000))
		return (1);
	if ((now.tv_usec  / 1000) < (deadline.tv_usec / 1000))
		return (-1);
	else
		return (0);
}

// TODO When dining they do what? Take forks to eat
// TODO When they sleep, what?j
// FIXME Why does the report state often not happen? I think it takes too long. Hold all else, how?
// FIXED? It is possible for the reporting to be interlaced.
// TODO I guess this has to be a loop that breaks when the meal condition is met.
// TODO Add checks for the ability to grab a fork and a pause or release when it fails
// Remember that each of these threads is independent but trying to access shared things,.
// Maybe first imagine the philosoper as individualists
// TODO Do I have to run lock a philosopher's record as well (what does that mean?)
// TODO Getting too long; break up the function stages (get forks, eat, sleep)
// TODO Need to check to see if a philo has died
// TODO Give this a better name than launch_phil
void	launch_phil(void *ptr)
{
	t_plato	p;
	struct timeval	now;

	p = (*((t_plato *) ptr));	// NOTE all these brackets, Cast to t_plato first, then deref.
	print_placecard(p);
	while ((p.eaten < p.data->appetite) || (p.data->appetite = -1))
	{
		// FIXED? Invalid read in the right fork part of the line below
		if ((pthread_mutex_lock(p.l_fork) == 0) && (pthread_mutex_lock(p.r_fork) == 0))
		{
			report_state(p, EAT);
			p.starve_at = ms_to_timeval(timeval_to_ms(p.starve_at) + p.data->die_time);
			p.eaten++;	// NOTE Does this record need to be locked while updating?
			usleep(p.data->eat_time * 1000);	// HACK this is wrong because we arent storing microseconds (yet)
			pthread_mutex_unlock(p.l_fork);
			pthread_mutex_unlock(p.r_fork);
			report_state(p, NAP);
			usleep(p.data->nap_time * 1000);	// HACK Wrong, should be in microseconds
		}
		else
		{
			pthread_mutex_unlock(p.l_fork);
			// FIXED? Unitialised value and an invalid read in line below.
			pthread_mutex_unlock(p.r_fork);
			report_state(p, HMM);
		}
		gettimeofday(&now, NULL);
		//FIXME THis check fails: a philo died at 139 when die_time was 300
		if (is_later(now, p.starve_at) == 1)
		{
			report_state(p, DIE);
			exit(EXIT_SUCCESS);
		}
	}
}

// FIXED Segfualt due to invalid write (nap_time, but maybe more gen problem as the thing moves)
// NOTE Fix was to allocate sizeof(t_table) before arriving here
// This fills the parameters into t_table struct
// TODO Error checking - clean up routine needed.
// NOTE If die_time is less than (eat_time + nap_time) the simulation is impossible
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
		gettimeofday(&dat->started, NULL);
		pthread_mutex_init(&dat->report, NULL);
	}
	else
		exit(EXIT_FAILURE);
	if ((dat->table_size < 0) || (dat->die_time < 0) || (dat->eat_time < 0) || (dat->nap_time < 0))
		exit(EXIT_FAILURE);
}

//. Allocate mempory for the philosopher things.
//Really hard not to call this "set table" I should get a prize
// allocate what is needed for one philo struct
// TODO What do I do with a place to put thread ID?(Nothing seesm to be OK so far)
// FIXED The lowest seat ID is supposed to be 1 not 0 (per assignment)
void	setup_philos(t_plato *phil,  t_table *rules)
{
	int	i;

	i = 0;
	while (i < (rules->table_size - 1))
	{
		phil[i].data = rules;
		phil[i].seat = i + 1;
		phil[i].eaten = 0;
		phil[i].starve_at = ms_to_timeval(timeval_to_ms(rules->started) + rules->die_time);
		i++;
	}
}

// Init and allocate an array of mutexes. Link them to each philosophers' L and R hands
// TODO Check that these are linked correctly to the philosophers
// TODO Special case set up for first and last philosopher's forks
// so let 0 rep their left fork and their seat no (+1), the right
// NOTE table size is the number of forks and philos. size -1 is the final index position
// Therefore the final index entry has to loop back to 0 for one fork
void	forks_laid(pthread_mutex_t *forks, t_plato *p)
{
	int	i;
	int	n;

	i = 0;
	n = p->data->table_size;
	while (i < n)
	{
		pthread_mutex_init(&forks[i], NULL);
		p[i].l_fork = &forks[i];
		p[i].r_fork = &forks[i + 1];
		i++;
	}
	pthread_mutex_init(&forks[n - 1], NULL);
	p[n - 1].l_fork = &forks[n - 1];
	p[n - 1].r_fork = &forks[0];
}

// Get parameters stored
// Create an array of mutexes to represent forks
// Set up philosoper characteristics
// link mutexes to philosophers
// Launch a thread for each philosopher
// WOnder why it is crashing this time.
// TODO something about timing - set a start time, store something in the philo
// TODO Try compilation without thread sanitize option
// TODO Function too long, some set up to move elsewhere.
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
