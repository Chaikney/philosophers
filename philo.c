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

// TODO Convert time parameters to an appropriate format (struct?)
int	main(int argc, char **argv)
{
    int			num_philos;
    struct timeval	die_time;
    struct timeval	eat_time;
    struct timeval	sleep_time;
    int			meals;

    if ((argc == 5) || (argc == 6))
    {
        num_philos = ph_atoi(argv[1]);
        die_time = ms_to_timeval(ph_atoi(argv[2]));
        eat_time = ms_to_timeval(ph_atoi(argv[3]));
        sleep_time = ms_to_timeval(ph_atoi(argv[4]));
        if (argc == 6)
            meals = ph_atoi(argv[5]);
        else
            meals = -1;
        printf("philos: %i\ndie after: %i\neat for: %i\nsleep for: %i\neat until: %i",
               num_philos, timeval_to_ms(die_time), timeval_to_ms(eat_time),
               timeval_to_ms(sleep_time), meals);
    }
    return (0);
}
