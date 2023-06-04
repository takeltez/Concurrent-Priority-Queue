#include "queue.h"

/**
 * void create_queue(void)
 * 
 * Create queue.
 * 
 * **/
void create_queue(void)
{
	int i, max;

	for(i = 0, max = 20; i < MAX_CHUNKS; max += 20, i++)
	{
		if (i == MAX_CHUNKS - 1)
		{
			create_chunk(UINT32_MAX);
		}
		else
			create_chunk(max);
	}
}

/**
 * void destroy_queue(void)
 * 
 * Destroy queue.
 * 
 * **/
void destroy_queue(void)
{
	if(!head)
	{
		return;
	}

	Chunk *c = head;
	Chunk *next = NULL;

	if(c->buffer)
	{
		free(c->buffer);
	}

	while(c)
	{
		next = c->next;

		free(c);

		c = next;
	}

	head = NULL;
}

/**
 * Chunk *init_chunk(States state, uint32_t max)
 * 
 * Initialize new chunk fields.
 * 
 * Parameters:
 * 	@state - chunk state.
 * 	@max - chunk maximal possible stored key.
 * 
 * Returned value:
 * 	@c - pointer to the new chunk.
 * 
 * **/
Chunk *init_chunk(States state, uint32_t max)
{
	Chunk *c = (Chunk*)malloc(sizeof(Chunk));

	c->max = max;

	memset(c->entries, 0, sizeof(c->entries));
	memset(c->frozen, 0, sizeof(c->frozen));

	c->status.state = state;
	c->status.index = 0;
	c->status.frozenInd = 0;

	c->status.aIncIdx = status_aIncIdx;
	c->status.isInFreeze = status_isInFreeze;
	c->status.getIdx = status_getIdx;
	c->status.CAS = status_CAS;
	c->status.aOr = status_aOr;
	c->status.aXor = status_aXor;
	c->status.set = status_set;
	c->status.getState = status_getState;

	c->entryFrozen = chunk_entryFrozen;
	c->markPtrs = chunk_markPtrs;

	c->next = NULL;
	c->buffer = NULL;

	return c;
}

/**
 * void create_chunk(uint32_t max)
 * 
 * Create new chunk.
 * 
 * Parameters:
 * 	@max - chunk maximal possible stored key.
 * 
 * **/
void create_chunk(uint32_t max)
{
	if(head == NULL)
	{
		Chunk *c = init_chunk(DELETE, max);

		head = c;

		return;
	}

	Chunk *tail = head;

	while(tail->next)
	{
		tail = tail->next;
	}

	Chunk *c = init_chunk(INSERT, max);

	tail->next = c;
}

/**
 * void print_queue(Chunk *root)
 * 
 * Print queue state.
 * 
 * Parameters:
 * 	@root - pointer to queue head.
 * 
 * **/
void print_queue(Chunk *root)
{
	if(!root)
	{
		printf("Queue is not allocated yet\n");

		return;
	}

	int i = 0;
	int j;

	Chunk *c = root;

	if(c->buffer)
	{
		printf("Buffer:\n\tStatus properties:\n\t\tstate = %u\n\t\tindex = %u\n\t\tfrozenInd = %u\n", 
		       c->buffer->status.state, c->buffer->status.index, c->buffer->status.frozenInd);

		printf("\tChunk properties:\n\t\tmax = %u\n", c->buffer->max);

		for(j = 0; c->buffer->entries[j] != 0 && j < M; j++)
		{
			printf("\t\tentries[%d] = %lu\n", j, c->buffer->entries[j]);
		}

			for (j = 0; c->buffer->frozen[j] != 0 && j < M_FROZEN; j++)
			{
				printf("\t\tfrozen[%d] = %lu\n", j, c->buffer->frozen[j]);
			}
	}

	while(c)
	{
		printf("Chunk[%d]:\n\tStatus propirties:\n\t\tstate = %u\n\t\tindex = %u\n\t\tfrozenInd = %u\n",
		       i, c->status.state, c->status.index, c->status.frozenInd);

		printf("\tChunk properties:\n\t\tmax = %u\n", c->max);

		for (j = 0; c->entries[j] != 0 && j < M; j++)
		{
			printf("\t\tentries[%d] = %lu\n", j, c->entries[j]);
		}

		for (j = 0; c->frozen[j] != 0 && j < M_FROZEN; j++)
		{
			printf("\t\tfrozen[%d] = %lu\n", j, c->frozen[j]);
		}

		c = c->next;
		i++;
	}
}

/**
 * bool getChunk(Chunk **cur, Chunk **prev)
 * 
 * Search for a required chunk in chunk list.
 * In case such chunk was found pointer to previous chunk stored into *prev.
 * 
 * Parameters:
 * 	@cur - double pointer to required chunk.
 * 	@prev - double pointer to previous chunk.
 * 
 * Returned value:
 * 	@true - in case chunk *cur was found in chunk list.
 * 	@false - in case chunk *cur was not found in chunk list.
 * 
 * **/
bool getChunk(Chunk **cur, Chunk **prev)
{
	Chunk *c = head;

	if(c == *cur)
	{
		return true;
	}

	while(c)
	{
		if(c->next == *cur)
		{
			*prev = c;

			return true;
		}
		c = c->next;
	}

	return false;
}

/**
 * void getChunkByKey(Chunk **cur, Chunk **prev, int key)
 * 
 * Search for a appropriate chunk in order to store a given key.
 * In case such chunk was found pointer to this chunk stored into *cur, and pointer to previous chunk stored into *prev.
 * 
 * Parameters:
 * 	@cur - double pointer on chunk for store given key.
 * 	@prev - double pointer on previous chunk.
 * 
 * **/
void getChunkByKey(Chunk **cur, Chunk **prev, int key)
{
	*cur = head;

	if((*cur)->max >= (uint32_t)key)
	{
		return;
	}

	while(*cur)
	{
		if((*cur)->next->max >= (uint32_t)key)
		{
			break;
		}
		*cur = (*cur)->next;
	}

	*prev = *cur;
	*cur = (*prev)->next;
}
/**
 * void keyCAS(uint64_t *mem, uint64_t old, uint64_t new)
 * 
 * Compare value stored in 'mem' with value 'old'.
 * If equal, update memory location 'mem' with value 'new'.
 * Otherwise, copy value from 'mem' into 'old'.
 * 
 * Parameters:
 * 	@mem - pointer to the memory region which is supposed to be updated.
 * 	@old - value which compare value stored in 'mem' with.
 * 	@new - value which should be coppied into 'mem'.
 * 
 * **/
void keyCAS(uint64_t *mem, uint64_t old, uint64_t new)
{
	__atomic_compare_exchange_n(mem, &old, new, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

/**
 * bool chunkCAS(Chunk **c, Chunk *cur, Chunk *local)
 * 
 * Compare chunk 'c' with chunk 'cur'.
 * If equal, chunk 'local' updates chunk 'c'.
 * Otherwise, chunk 'c' updates chunk 'cur'.
 * 
 * Parameters:
 * 	@c - double pointer to the chunk which is supposed to be updated.
 * 	@cur - pointer to the chunk which compare chunk 'c' with.
 * 	@local - pointer to chuck which should update chunk 'c'.
 * 
 * Returned value:
 * 	@true - if chunks 'c' and 'cur' equal.
 * 	@false - otherwise.
 * 
 * **/
bool chunkCAS(Chunk **c, Chunk *cur, Chunk *local)
{
	return __atomic_compare_exchange_n(c, cur, local, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

/**
 * bool createBuffer(int key, Chunk *c, Chunk **buf)
 * 
 * Create buffer for the first chunk, add key into this buffer and store pointer to this buffer into *buf.
 * 
 * Parameters:
 * 	@key - key to be added into buffer.
 * 	@c - pointer on the first chunk.
 * 	@buf -double pointer to the buffer
 * 
 * Returned value:
 * 	@true - the new buffer was successfully connected to the first chunk.
 * 	@false - another thread had connected another buffer.
 * 
 * **/
bool createBuffer(int key, Chunk *c, Chunk **buf)
{
	bool buf_status;

	// Only one buffer allocated for all threads
	#pragma omp critical
	{
		// If buffer has been already allocated by any previous thread
		if(c->buffer)
		{
			buf_status = true;
		}
		else
		{
			c->buffer = init_chunk(BUFFER, 20);
			buf_status = false;
		}
	}

	if(buf_status)
	{
		return false;
	}

	int i;

	for(i = 0; c->buffer->entries[i] != 0 && i < M; i++);

	c->buffer->entries[i] = key;

	*buf = c->buffer;

	return true;
}

/**
 * int getIdx(Status s)
 * 
 * Get chunk status index.
 * 
 * Parameters:
 * 	@s - chunk status.
 * 
 * Returned value:
 * 	@s.index - chunk status index.
 * 
 * **/
int getIdx(Status s)
{
	return s.index;
}

/**
 * Chunk *split(Chunk *c)
 * 
 * Create two new half-full chunks from a signle full frozen chunk.
 * The first chunk, with the lower-valued part of the keys, points to the second chunk, with the higher-valued part.
 * 
 * Parameters:
 * 	@c - pointer to the single full frozen chunk.
 * 
 * Returned value:
 * 	@c - pointer to the new first chunk.
 * 
 * **/

Chunk *split(Chunk *c)
{
	States state;
	int i, frt_idx, sec_idx;
	uint32_t max_key_fir;
	uint32_t max_key_sec;
	Chunk *first, *second;

	// Set max key of the second chunk as max key of the split chunk
	max_key_sec = c->max;

	// If we split last chunk whose max key greater than max key of previous chunk more than 20
	if(max_key_sec == UINT32_MAX)
	{
		// Set max key of the first chunk as half of max key of the second chunk
		max_key_fir = max_key_sec / 2;
	}
	else
	{
		// Otherwise, set max key of the first chunk as max key of the second chunk minus 10
		max_key_fir = max_key_sec - 10;
	}

	state = c->status.getState(&c->status);

	first = init_chunk(state, max_key_fir);
	second = init_chunk(state, max_key_sec);

	// Copy keys from split chunk
	for(i = 0, frt_idx = 0, sec_idx = 0; c->entries[i] != 0 && i < M; i++)
	{
		// If current key less or equal than max key of first chunk
		if(c->entries[i] <= first->max)
		{
			// Copy current key in the first chunk and increment index
			frt_idx = first->status.getIdx(&first->status);
			first->entries[frt_idx] = c->entries[i];
			first->status.aIncIdx(&first->status);
		}
		else // Otherwise
		{
			// Copy current key in the second chunk and increment index
			sec_idx = second->status.getIdx(&second->status);
			second->entries[sec_idx] = c->entries[i];
			second->status.aIncIdx(&second->status);
		}
	}

	first->next = second;

	return first;
}

/**
 * Chunk *mergeFirstChunk(Chunk *c)
 * 
 * Create a new first chunk with M ordered keys taken from the frozen first chunk, the buffer and from the second chunk.
 * If there are too many frozen keys, a new first chunk and new second chunk can be created.
 * 
 * Parameters:
 * 	@c -pointer to the single full frozen chunk.
 * 
 * Returned value:
 * 	@c - pointer to the new first chunk.
 * 
 * **/
Chunk *mergeFirstChunk(Chunk *c)
{
	States state;
	int i, idx;
	int merges_left;
	uint32_t max_key;
	Chunk *merged, *tail;
	Chunk *new, *cur;

	// Set max key of new first chunk as max key of second chunk
	max_key = c->next->max;

	state = c->status.getState(&c->status);
	new = init_chunk(state, max_key);

	merged = new;

	for(merges_left = 3; merges_left; merges_left--)
	{
		switch(merges_left)
		{
			case 3: // Copy from first chunk
				cur = c;
				break;
			case 2: // Copy from buffer
				cur = c->buffer;
				break;
			case 1: // Copy from second chunk
				cur = c->next;
				break;
		}

		// Copy keys from the current chunk
		for(i = 0; cur->entries[i] != 0 && i < M && idx < M; i++)
		{
			idx = new->status.getIdx(&new->status);

			new->entries[idx] = cur->entries[i];
			new->status.aIncIdx(&new->status);

			idx = new->status.getIdx(&new->status);
		}

		// If there is no space left in new chunk
		if(new->status.getIdx(&new->status) == M)
		{
			// If copy isn't finished
			if(cur->entries[i] != 0 && i < M)
			{
				// Allocate another new chunk
				new = init_chunk(state, max_key);
				tail = merged;

				//Find tail of new chunk
				while(tail->next)
				{
					tail = tail->next;
				}

				// Add another new chunk to previous new chunk
				tail->next = new;

				idx = new->status.getIdx(&new->status);

				// Continue copy keys from the current chunk
				for(; cur->entries[i] != 0 && i < M && idx < M; i++)
				{
					idx = new->status.getIdx(&new->status);

					new->entries[idx] = cur->entries[i];
					new->status.aIncIdx(&new->status);

					idx = new->status.getIdx(&new->status);
				}
			}
		}
	}

	return merged;
}
