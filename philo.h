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
typedef struct s_table
{
	u_int64_t	die_time;
	u_int64_t	nap_time;
	u_int64_t	eat_time;
	int	table_size;
	int	appetite;
	pthread_mutex_t	report;
	struct timeval	started;
}	t_table;

// Platonic ideal of philospher
// Each thread can keep track of their state and needs here.
// Links to the unchanging data, I guess so we set it up.
// TODO Add stuff back to this - the locks they have? "left and right hand?"
typedef struct s_plato
{
	t_table	*data;
	int	seat;
	pthread_t	id;
	int	eaten;
	struct timeval	starve_at;
	pthread_mutex_t	*l_fork;	// With these two, is it linked to anything? Or just pointing?
	pthread_mutex_t	*r_fork;	// How does the identification with the forks work? They must exist
}	t_plato;

typedef struct	s_logmsg
{
	int	state;
	struct timeval	event_time;
	int	philo;
}	t_logmsg;

// setup.c - read parameters, initialise variables, etc.
void	forks_laid(pthread_mutex_t *forks, t_plato *p);
void	setup_philos(t_plato *phil,  t_table *rules);
void	get_general_data(t_table *dat, int argc, char **argv);

// helper functions
int	ph_atoi(char *str);
u_int64_t	timeval_to_ms(struct timeval t);
struct timeval	ms_to_timeval(u_int64_t t);
u_int64_t ms_after(struct timeval t1, struct timeval t2);
struct timeval	add_ms(struct timeval t1, u_int64_t ms);
int	is_later(struct timeval now, struct timeval deadline);

// logging
void	report_state(t_plato phil, int state);

// actions
void	take_forks(t_plato p);
void	eat_food(t_plato p);
void	replace_forks_and_nap(t_plato p);

// debugging functions that can be removed (or dectivated) later
void	print_placecard(t_plato p);
void	print_menu(t_table t);
#endif
