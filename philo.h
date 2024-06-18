// TODO Add 42 header
// NOTE If LSP auto-includes bits/pthreadtypes.h, GET RID OF IT

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>	// printf
# include <sys/time.h>	// timeval struct, getttimeofday
# include <sys/types.h>
# include <pthread.h>	// phtreads
# include <stdlib.h>	//malloc
# include <unistd.h>	// usleep

// Philosopher state constants to make the reporting easier to read in code
# define HAS 1
# define EAT 2
# define NAP 3
# define HMM 4
# define DIE 5

// The shared conditions that they are dining at,
// i.e. the table rule.
typedef	struct s_table
{
    int	die_time;
    int	nap_time;
    int	eat_time;
    int	table_size;
    int	appetite;
    pthread_mutex_t	report;
    struct timeval	started;
}	t_table;

// Platonic ideal of philospher
// Each thread can keep track of their state and needs here.
// Links to the unchanging data, I guess so we set it up.
// TODO Add stuff back to this - the locks they have? "left and right hand?"
typedef	struct s_plato
{
    t_table	*data;
    int	seat;
    pthread_t	id;
    int	eaten;
    pthread_mutex_t	*l_fork;	// With these two, is it linked to anything? Or just pointing?
    pthread_mutex_t	*r_fork;	// How does the identification with the forks work? They must exist
}	t_plato;

// helper functions
int	ph_atoi(char *str);
int	timeval_to_ms(struct timeval t);
struct timeval	ms_to_timeval(int t);

// logging
void	report_state(t_plato phil, int state);

// debugging functions that can be removed (or dectivated) later
void	print_placecard(t_plato p);
void	print_menu(t_table t);
#endif
