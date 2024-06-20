#include "philo.h"

// TODO Add 42 header

// NOTE Fix was to allocate sizeof(t_table) before arriving here
// This fills the parameters into t_table struct
// TODO Error checking - clean up routine needed.
// DONE Restore error checking, allowing for -1 (invalid) from ph_atoi
// NOTE If die_time is less than (eat_time + nap_time) the simulation is impossible
// FIXED Also handle values of 0 - all ilegitimate?
void	get_general_data(t_table *dat, int argc, char **argv)
{
	int64_t	tmp;
	int		i;

	if ((argc == 5) || (argc == 6))
	{
		i = 1;
		while (i <= (argc - 1))
		{
			tmp = ph_atoi(argv[i++]);
			if (tmp <= 0)
				exit(EXIT_FAILURE);
		}
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
}

// Set initial values for the philosopher things.
// Really hard not to call this "set table" I should get a prize
// allocate what is needed for one philo struct
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
