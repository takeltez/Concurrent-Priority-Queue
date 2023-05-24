#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	int i, max;
	Chunk *curbuf = NULL;
	Chunk *cur = NULL;

	/* Create three chunks */

	for(i = 0, max = 20; i < MAX_CHUNKS; max += 20, i++)
	{
		if (i == MAX_CHUNKS - 1)
		{
			create_chunk(UINT32_MAX);
		}
		else
			create_chunk(max);
	}

	/* Following code just for test */

	cur = head;

	/* Create first chunk's buffer and add key 4 */

	printf("Queue state:\n");

	createBuffer(4, cur, &curbuf);

	/* Add keys 5 and 14 to buffer */

	curbuf->entries[1] = 5;
	curbuf->entries[2] = 14;

	/* Print queue state */

	print_queue(head);

	/* Remove second chunk */

	printf("\nRemoving second chunk...\n\n");

	cur->markPtrs(cur->next);

	/* Print queue state */

	printf("Queue state:\n");

	print_queue(head);

	return 0;
}
