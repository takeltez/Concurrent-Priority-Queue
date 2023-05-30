#include "queue.h"

Chunk *head = NULL;

void insert(int key)
{
	(void) key;

	// Create pointers to current and previous chunks
	Chunk *cur = NULL;
	Chunk *prev = NULL;

	while(true) {

		// Set the current and previous chunk pointers
		getChunk_by_key(&cur, &prev, key);

		// If it is the first chunk, insert in the buffer instead
		if(cur == head) {
			printf("Head\n");

			if(insertToBuffer(key, cur, head)) return;
			else continue;
		}
	}

	/* TODO */
}

int deleteMin(void)
{
	/* TODO */

	return 0;
}
