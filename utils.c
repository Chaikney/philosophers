#include "philo.h"

// TODO Add 42 header

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

// handle the parameters, must only be positive ints
// Return -1 if the input is invalid.
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

// Return the number of millseconds between two timevals
// Return is positive if t1 is later than t2
int	ms_diff(struct timeval t1, struct timeval t2)
{
	int	ms;
	int	tmp;

	ms = (t1.tv_sec - t2.tv_sec) * 1000;
	tmp = (t1.tv_usec / 1000) - (t2.tv_usec / 1000);
	ms = ms + tmp;
	return (ms);
}

// Add a number of milliseconds to the supplied timeval
// Return timeval. Works with a -ve value of ms
struct timeval	add_ms(struct timeval t1, int ms)
{
	struct timeval	t2;

	t2.tv_sec = t1.tv_sec + (ms / 1000);
	t2.tv_usec = t1.tv_usec + ((ms % 1000) * 1000);
	return (t2);
}
