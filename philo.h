// TODO Add 42 header

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>	// printf
# include <sys/time.h>	// timeval struct, getttimeofday
# include <pthread.h>	// phtreads

// helper functions
int	ph_atoi(char *str);
int	timeval_to_ms(struct timeval t);
struct timeval	ms_to_timeval(int t);

// logging
void	report_state(int phil, int state);

#endif
