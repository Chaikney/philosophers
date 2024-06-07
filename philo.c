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

int	main(int argc, char **argv)
{
    int	num_philos;
    int	die_time;
    int	eat_time;
    int	sleep_time;
    int	meals;

    if ((argc == 5) || (argc == 6))
    {
        num_philos = ph_atoi(argv[1]);
        die_time = ph_atoi(argv[2]);
        eat_time = ph_atoi(argv[3]);
        sleep_time = ph_atoi(argv[4]);
        if (argc == 6)
            meals = ph_atoi(argv[5]);
        else
            meals = -1;
        printf("philos: %i\ndie after: %i\neat for: %i\nsleep for: %i\neat until: %i",
           num_philos, die_time, eat_time, sleep_time, meals);
    }
    return (0);
}
