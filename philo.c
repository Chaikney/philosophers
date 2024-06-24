/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 09:43:45 by chaikney          #+#    #+#             */
/*   Updated: 2024/06/24 09:43:49 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// A single philosopher only has one fork and will starve.
void	dining_alone(void *ptr)
{
	t_plato	p;

	p = (*((t_plato *) ptr));
	pthread_mutex_lock(p.l_fork);
	log_action(p, make_msg(HAS, p.seat));
	while (p.data->stop == 0)
		take_pulse(&p);
}

// A loop that breaks when the meal condition is met.
// Remember that each thread is independent but trying to access shared things.
// NOTE all these brackets, Cast to t_plato first, then deref.
void	dining_loop(void *ptr)
{
	t_plato	p;

	p = (*((t_plato *) ptr));
	while (p.data->stop == 0)
	{
		take_forks(&p);
		eat_food(&p);
		replace_forks_and_nap(p);
		log_action(p, make_msg(HMM, p.seat));
		take_pulse(&p);
	}
}

// Launch and manage a single thread.
// This, unlike the main case, gets detached and waited for.
// Without that the main program ends before we can report on this philo.
void	sartre_mode(t_plato *philo)
{
	pthread_create(&philo[0].id, NULL, (void *) &dining_alone, &philo[0]);
	pthread_detach(philo[0].id);
	while (philo->data->stop == 0)
		usleep(100);
}

// Launch and manage multiple threads. The main case.
void	olstrom_mode(t_plato *philo)
{
	int	i;
	int	n;

	n = (philo[0].data->table_size);
	i = 1;
	while (i <= n)
	{
		pthread_create(&philo[i - 1].id, NULL,
			(void *) &dining_loop, &philo[i - 1]);
		i++;
	}
	i = 0;
	while (i < (n - 1))
		pthread_join(philo[i++].id, NULL);
}

// Get parameters stored
// Create an array of mutexes to represent forks
// Set up philosoper characteristics
// link mutexes to philosophers
// Launch a thread for each philosopher
int	main(int argc, char **argv)
{
	t_plato			*philo;
	t_table			*house_rules;
	pthread_mutex_t	*forks;

	if (check_data(argc, argv) == 0)
	{
		house_rules = get_general_data(argc, argv);
		philo = setup_philos(house_rules);
		forks = forks_laid(philo, house_rules->table_size);
		if (house_rules->table_size == 1)
			sartre_mode(philo);
		else
			olstrom_mode(philo);
		clear_table(forks, philo, house_rules);
	}
	return (0);
}
