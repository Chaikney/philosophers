/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 09:44:49 by chaikney          #+#    #+#             */
/*   Updated: 2024/06/24 09:44:52 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// Read parameters and return -1 if the parameteres are invalid.
// NOTE This includes an impossible situation like:
// If die_time is less than (eat_time + nap_time)
// HACK That check is so inefficient!
int	check_data(int argc, char **argv)
{
	int64_t	tmp;
	int		i;

	if ((argc < 5) || (argc > 6))
		return (-1);
	i = 1;
	while (i <= (argc - 1))
	{
		tmp = ph_atoi(argv[i++]);
		if (tmp <= 0)
			return (-1);
	}
	if (ph_atoi(argv[2]) < (ph_atoi(argv[3]) + ph_atoi(argv[4])))
	{
		printf("\nImpossible conditions");
		return (-1);
	}
	return (0);
}

// This fills the parameters into t_table struct
// NOTE The parameters must have passed check_data first
// FIXED? A table size of 1 leads to segfualts elsewhere.
// FIXED Function get_general_data is too long
// FIXED? exit is a forbidden function, remove it.
t_table	*get_general_data(int argc, char **argv)
{
	t_table	*dat;

	dat = malloc(sizeof(t_table));
	dat->table_size = ph_atoi(argv[1]);
	dat->die_time = (ph_atoi(argv[2]));
	dat->eat_time = (ph_atoi(argv[3]));
	dat->nap_time = (ph_atoi(argv[4]));
	dat->sated = 0;
	dat->stop = 0;
	if (argc == 6)
		dat->appetite = ph_atoi(argv[5]);
	else
		dat->appetite = -1;
	gettimeofday(&dat->started, NULL);
	pthread_mutex_init(&dat->report, NULL);
	pthread_mutex_init(&dat->update, NULL);
	return (dat);
}

// Set initial values for the philosopher things.
// Really hard not to call this "set table" I should get a prize
// allocate what is needed for arrary of philo structs
t_plato	*setup_philos(t_table *rules)
{
	int		i;
	t_plato	*phil;

	i = 0;
	phil = malloc(sizeof(t_plato) * rules->table_size);
	while (i < (rules->table_size))
	{
		phil[i].id = 0;
		phil[i].data = rules;
		phil[i].seat = i + 1;
		phil[i].eaten = 0;
		phil[i].starve_at = add_ms(rules->started, rules->die_time);
		phil[i].is_dead = 0;
		phil[i].is_sated = 0;
		i++;
	}
	return (phil);
}

// Init and allocate an array of mutexes.
// Link them to each philosophers' L and R hands
// TODO Check that these are linked correctly to the philosophers
// NOTE table size is the number of forks and philos.
// size -1 is the final index position
// Therefore the final index entry has to loop back to 0 for one fork
// FIXED One philo = one fork, this does not work!
pthread_mutex_t	*forks_laid(t_plato *p, int n)
{
	int				i;
	pthread_mutex_t	*forks;

	i = 0;
	forks = malloc(sizeof(pthread_mutex_t) * n);
	if (n == 1)
	{
		pthread_mutex_init(&forks[i], NULL);
		p[i].l_fork = &forks[i];
		p[i].r_fork = &forks[i];
		return (forks);
	}
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
	return (forks);
}

// Destroy mutexes, free memory, etc to ensure that we finish the sim cleanly
void	clear_table(pthread_mutex_t *forks, t_plato *philos, t_table *rules)
{
	int	n;
	int	i;

	pthread_mutex_lock(&rules->update);
	n = rules->table_size;
	i = 0;
	while (i < n)
		pthread_mutex_destroy(&forks[i++]);
	pthread_mutex_unlock(&rules->update);
	pthread_mutex_destroy(&rules->report);	// FIXME warning of possible data race but here, surely not
	pthread_mutex_destroy(&rules->update);
	free(forks);
	free(rules);
	free(philos);
}
