#include "philo.h"

// TODO Add 42 header

// A collection of general-purpose functions that do not
// manipulate external data (and so do not need mutexes?)

// Convert a timeval struct into an int representing a number of
// milliseconds
u_int64_t	timeval_to_ms(struct timeval t)
{
	u_int64_t	milliseconds;

	milliseconds = 0;
	milliseconds += t.tv_sec * 1000;
	milliseconds += t.tv_usec / 1000;
	return (milliseconds);
}

// Convert the given times in milliseconds to the timeval struct
// which uses seconds and microseconds
struct timeval	ms_to_timeval(u_int64_t t)
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
u_int64_t	ms_diff(struct timeval t1, struct timeval t2)
{
	u_int64_t	ms;
	u_int64_t	tmp;

	ms = (t1.tv_sec - t2.tv_sec) * 1000;
	tmp = (t1.tv_usec / 1000) - (t2.tv_usec / 1000);
	ms = ms + tmp;
	return (ms);
}

// Add a number of milliseconds to the supplied timeval
// Return timeval. Works with a -ve value of ms
struct timeval	add_ms(struct timeval t1, u_int64_t ms)
{
	struct timeval	t2;

	t2.tv_sec = t1.tv_sec + (ms / 1000);
	t2.tv_usec = t1.tv_usec + ((ms % 1000) * 1000);
	return (t2);
}

// Return 1 if the first time is later than deadline
// -1 if not
// 0 if equal (to millisecond precision(?))
// First compare seconds then if equal look at the microseconds
// TODO May be un-needed and could be removed later.
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
