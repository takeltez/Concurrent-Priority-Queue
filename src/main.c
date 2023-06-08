#include "bench.h"
#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	create_queue();

	fill_queue(10000);

	printf("Queue before deletion\n\n");
	print_queue(head);

	for(int i = 0; i <= 1000; i++)
	{
		deleteMin();
	}

	printf("\nQueue after deletion\n\n");
	print_queue(head);

	destroy_queue();

	return 0;
}
