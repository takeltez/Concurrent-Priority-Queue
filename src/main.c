#include "queue.h"

int main(int argc, const char **argv)
{
	(void) argc;
	(void) argv;

	int i, max;
	Chunk *curbuf = NULL;
	Chunk *cur = NULL;

	for(i = 0, max = 20; i < MAX_CHUNKS; max += 20, i++)
	{
		if (i == MAX_CHUNKS - 1)
		{
			create_chunk(UINT32_MAX);
		}
		else
			create_chunk(max);
	}

	cur = head;

	createBuffer(4, cur, &curbuf);

	curbuf->entries[1] = 5;

	print_queue(curbuf);

	return 0;
}
