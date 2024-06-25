/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/24 09:44:00 by chaikney          #+#    #+#             */
/*   Updated: 2024/06/24 09:44:10 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// NOTE If LSP auto-includes bits/pthreadtypes.h, GET RID OF IT

#ifndef PHILO_H
# define PHILO_H
# include <stdio.h>	// printf
# include <sys/time.h>	// timeval struct, getttimeofday
# include <sys/types.h>
# include <pthread.h>	// pthreads, mutexes, etc
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
// Some of these values are constant;
// others are incremented (or flipped) and therefore need to be locked:
// - sated
// - living
typedef struct s_table
{
	u_int64_t		die_time;
	u_int64_t		nap_time;
	u_int64_t		eat_time;
	int				table_size;
	int				appetite;
	int				sated;
	int				stop;
	pthread_mutex_t	report;
	pthread_mutex_t	update;
	struct timeval	started;
}	t_table;

// Platonic ideal of philospher
// Each thread can keep track of their state and needs here.
// Links to the unchanging data, I guess so we set it up.
// NOTE Do I need a mutex for update to this?
// I think the data in here is *not* shared, so no
// It is only accessed by the one thread.
typedef struct s_plato
{
	t_table			*data;
	int				seat;
	pthread_t		id;
	int				eaten;
	int				is_dead;
	int				is_sated;
	struct timeval	starve_at;
	pthread_mutex_t	*l_fork;
	pthread_mutex_t	*r_fork;
}	t_plato;

typedef struct s_logmsg
{
	int				state;
	struct timeval	event_time;
	int				philo;
}	t_logmsg;

// setup.c - read parameters, initialise variables, etc.
pthread_mutex_t	*forks_laid(t_plato *p, int n);
t_plato			*setup_philos(t_table *rules);
t_table			*get_general_data(int argc, char **argv);
int				check_data(int argc, char **argv);
void			clear_table(pthread_mutex_t *forks,
					t_plato *philos, t_table *rules);

// dining modes and their thread functions
void			sartre_mode(t_plato *philo);
void			dining_alone(void *ptr);
void			olstrom_mode(t_plato *philo);
void			dining_loop(void *ptr);

// helper functions
int64_t			ph_atoi(char *str);
u_int64_t		timeval_to_ms(struct timeval t);
struct timeval	ms_to_timeval(u_int64_t t);
u_int64_t		ms_after(struct timeval t1, struct timeval t2);
struct timeval	add_ms(struct timeval t1, u_int64_t ms);

// logging
t_logmsg		*make_msg(int state, int seat);
void			log_action(t_plato p, t_logmsg *msg);
int				all_done(t_plato *p, int flag);
void			take_pulse(t_plato *p);

int	getset_stop(t_plato *p, int flag);
// actions
void			take_forks(t_plato *p);
void			eat_food(t_plato *p);
void			replace_forks_and_nap(t_plato p);
#endif
