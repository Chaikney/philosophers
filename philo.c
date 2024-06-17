#include "philo.h"
#include <pthread.h>

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

// Debug function to print philosopher characteristics
void	print_thesis(t_plato p)
{
	printf("philos: %i\tSitting at: %i\ndie after: %i\neat for: %i\nsleep for: %i\neat until: %i",
			p.table_size, p.seat, timeval_to_ms(p.die_time), timeval_to_ms(p.eat_time),
			timeval_to_ms(p.nap_time), p.appetite);
}

void	launch_phil(void *ptr)
{
	t_plato	p;

	p = (*((t_plato *) ptr));	// NOTE all these brackets, Cast to t_plato first, then deref.
	print_thesis(p);
}

// DONE Convert time parameters to an appropriate format (struct?)
int	main(int argc, char **argv)
{
	int			num_philos;
	int	i;
    pthread_t	*phil;
	t_plato		philcharacter;

	if ((argc == 5) || (argc == 6))
	{
		num_philos = ph_atoi(argv[1]);
		philcharacter.die_time = ms_to_timeval(ph_atoi(argv[2]));
		philcharacter.eat_time = ms_to_timeval(ph_atoi(argv[3]));
		philcharacter.nap_time = ms_to_timeval(ph_atoi(argv[4]));
		if (argc == 6)
			philcharacter.appetite = ph_atoi(argv[5]);
		else
			philcharacter.appetite = -1;
		philcharacter.table_size = num_philos;
		i = 1;
		phil = NULL;
		while (i <= num_philos)
		{
			// TODO Confirm that this format creates treads with mstly-identical data and unique seat
			philcharacter.seat = i;
			if (pthread_create(phil, NULL, (void *) &launch_phil, &philcharacter) != 0)
				printf("Failed to create thread!");
			i++;
		}

		// NOTE the join call waits for all the threads to arrive. Without this the program
		// gets to the end before thread 2 finishes.
		// NOTE I don't know if this form would work.
		pthread_join(*phil, NULL);
	}
	return (0);
}
