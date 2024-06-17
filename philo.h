// TODO Add 42 header

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>	// printf
# include <sys/time.h>	// timeval struct, getttimeofday
# include <pthread.h>	// phtreads
# include <stdlib.h>	//malloc

// Platonic ideal of philospher
// Each thread can keep track of their state and needs here.
typedef	struct s_plato
{
    struct timeval	die_time;
    struct timeval	nap_time;
    struct timeval	eat_time;
    int	appetite;
    int	seat;
    int	table_size;
}	t_plato;

// helper functions
int	ph_atoi(char *str);
int	timeval_to_ms(struct timeval t);
struct timeval	ms_to_timeval(int t);

// logging
void	report_state(int phil, int state);

#endif
