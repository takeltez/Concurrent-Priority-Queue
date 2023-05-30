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

		Status s = cur->status.aIncIdx(&s);
		int idx = getIdx(s);

		// insert into a non-full and non-frozen chunk
		if(idx<M && !s.isInFreeze(&s)) {

			cur->entries[idx] = key;

			#pragma omp barrier

			if(!cur->status.isInFreeze(&cur->status)) return;

			if(!cur->entryFrozen(cur, idx)) return; // key got copied

		}

		// restructure the CBQP, then retry
		freezeChunk(cur);
		freezeRecovery(cur, prev);
	}
}

int deleteMin(void)
{
	/* TODO */

	return 0;
}
