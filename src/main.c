#include "bench.h"
#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	create_queue();

	insert(1);
	insert(2);
	insert(3);

	insert(23);
	insert(25);
	insert(36);

	insert(124);
	insert(224);
	insert(344);

	print_queue(head);

	printf("Min key#1: %u\n", deleteMin());
	printf("Min key#2: %u\n\n", deleteMin());

	print_queue(head);

	insert(15);

	print_queue(head);

	destroy_queue();

	return 0;
}
