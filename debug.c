#include "philo.h"

// TODO Add 42 header
// Debug function to print the house rules
void	print_menu(t_table t)
{
	printf("Booking for %i philophers\ndie after: %lu\neat for: %lu\nnap for: %lu",
		t.table_size, t.die_time, t.eat_time, t.nap_time);
	if (t.appetite == -1)
		printf("\nBINGE MODE");
	else
		printf("\nthis will be a %i course meal", t.appetite);
	printf("\nTimer starts at %lu millsecs(?)", timeval_to_ms(t.started));
}

//Debug function to print things about the philosopher
void	print_placecard(t_plato p)
{
	printf("\nSitting in place: %i", p.seat);
	printf("\tHas eaten %i meals", p.eaten);
	printf("\tWill die at %lu ms", timeval_to_ms(p.starve_at));
}
