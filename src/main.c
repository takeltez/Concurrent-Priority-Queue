#include "bench.h"
#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	create_queue();

	fill_queue(1000);

	print_queue(head);

	printf("Min key#1: %u\n", deleteMin());
	printf("Min key#2: %u\n", deleteMin());
	printf("Min key#3: %u\n", deleteMin());
	printf("Min key#4: %u\n", deleteMin());
	printf("Min key#5: %u\n", deleteMin());

	print_queue(head);

	destroy_queue();

	return 0;
}
