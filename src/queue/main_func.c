#include "queue.h"

Chunk *head = NULL;

/**
 * void insert(int key)
 * 
 * Insert key into queue.
 * 
 * Parameters:
 * 	@key - key to insert.
 * 
 * **/
void insert(int key)
{
	// Create pointers to current and previous chunks
	Chunk *cur = NULL;
	Chunk *prev = NULL;

	// All threads start at the same time
	#pragma omp barrier

	while(1)
	{
		// Set the current and previous chunk pointers
		getChunk_by_key(&cur, &prev, key);

		// If it is the first chunk, insert in the buffer instead
		if(cur == head)
		{
			if(insertToBuffer(key, cur))
			{
				return;
			}
			else
			{
				continue;
			}
		}

		Status s = cur->status.aIncIdx(&cur->status);
		int idx = getIdx(s);

		// insert into a non-full and non-frozen chunk
		if(idx < M && !s.isInFreeze(&s))
		{
			cur->entries[idx] = key;

			#pragma omp barrier

			if(!cur->status.isInFreeze(&cur->status))
			{
				return;
			}

			if(!cur->entryFrozen(cur, idx))
			{
				return; // key got copied
			}
		}

		// restructure the CBQP, then retry
		freezeChunk(cur);
		freezeRecovery(cur, prev);
	}
}

/**
 * int deleteMin(void)
 * 
 * Delete key with higher priority from queue.
 * 
 * Returned value:
 * 	@cur->entries[idx] - index of removed key.
 * 
 * **/
int deleteMin(void)
{
	Chunk *cur;
	Status s;
	int idx;

	// All threads start at the same time
	#pragma omp barrier

	while(1)
	{
		cur = head;
		s = cur->status.aIncIdx(&cur->status);
		idx = getIdx(s);

		if(idx < M && !s.isInFreeze(&s))
		{
			return cur->entries[idx];
		}

		freezeChunk(cur);
		freezeRecovery(cur, NULL);
	}

	return 0;
}
