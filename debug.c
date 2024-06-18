#include "philo.h"

// TODO Add 42 header
// Debug function to print the house rules
void	print_menu(t_table t)
{
	printf("Booking for %i philophers\ndie after: %i\neat for: %i\nnap for: %i",
		t.table_size, t.die_time, t.eat_time, t.nap_time);
	if (t.appetite == -1)
		printf("\nBINGE MODE");
	else
		printf("\nthis will be a %i course meal", t.appetite);
	printf("\nTimer starts at %i millsecs(?)", timeval_to_ms(t.started));
}

//Debug function to print things about the philosopher
void	print_placecard(t_plato p)
{
	printf("\nSitting in place: %i", p.seat);
	printf("\tHas eaten %i meals", p.eaten);
}
