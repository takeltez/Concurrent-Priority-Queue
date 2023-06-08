#include "queue.h"
#include "bench.h"

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

	int i = 1;
	int j;

	Chunk *c = root;

	while(c)
	{
		printf("Chunk[%d]:\n\tStatus propirties:\n\t\tstate = %u\n\t\tindex = %u\n\t\tfrozenInd = %u\n",
		       i, c->status.state, c->status.index, c->status.frozenInd);

		printf("\tChunk properties:\n\t\tmax = %u\n", c->max);

		printf("\t\tKeys:\n");

		for (j = 0; c->entries[j] != 0 && j < M; j++)
		{
			printf("\t\t\tentries[%d] = %lu\n", j, c->entries[j]);
		}

		c = c->next;
		i++;
	}
}

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
 * Status init_status(States state)
 * 
 * Initialize status fields.
 * 
 * Parameters:
 * 	@state - chunk state.
 * 
 * Returned value:
 * 	@status - new status structure.
 * 
 * **/
Status init_status(States state)
{
	Status status;

	status.state = state;
	status.index = 0;
	status.frozenInd = 0;

	status.aIncIdx = status_aIncIdx;
	status.aIncFrzIdx = status_aIncFrzIdx;
	status.isInFreeze = status_isInFreeze;
	status.getIdx = status_getIdx;
	status.CAS = status_CAS;
	status.aOr = status_aOr;
	status.aXor = status_aXor;
	status.set = status_set;
	status.getState = status_getState;

	return status;
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

	c->status = init_status(state);

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
	return  __atomic_compare_exchange_n(c, &cur, local, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
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
	int idx;

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

	idx = getIdx(c->buffer->status);
	c->buffer->entries[idx] = key;

	c->buffer->status.aIncIdx(&c->buffer->status);

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
 * int getFrzIdx(Status s)
 * 
 * Get chunk status frozen index.
 * 
 * Parameters:
 * 	@s - chunk status.
 * 
 * Returned value:
 * 	@s.index - chunk status frozen index.
 * 
 * **/
int getFrzIdx(Status s)
{
	return s.frozenInd;
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
	Chunk *first, *second;
	int i, frt_idx, sec_idx;
	uint32_t max_key;


	// Unmark chunk for split
	c->markPtrs(c);

	state = INSERT;
	max_key = c->max;

	first = init_chunk(state, max_key);
	second = init_chunk(state, max_key);

	// Sort keys in split chunk
	sort(c);

	// Copy keys from split chunk
	for(i = 0; i < M; i++)
	{
		// Copy lower-valued keys into first chunk
		if(i <= M / 2)
		{
			// Copy current key in the first chunk and increment index
			frt_idx = first->status.getIdx(&first->status);

			first->entries[frt_idx] = c->entries[i];
			first->status.aIncIdx(&first->status);
		}
		else // Copy hihger-valued keys into second chunk
		{
			// Copy current key in the second chunk and increment index
			sec_idx = second->status.getIdx(&second->status);

			second->entries[sec_idx] = c->entries[i];
			second->status.aIncIdx(&second->status);
		}
	}

	// Connect second chunk to the first chunk
	first->next = second;

	// Connect all other chunks to the second chunk
	if(c->next)
	{
		second->next = c->next;
	}
	else
	{
		second->next = NULL;
	}

	// Once split is finished, mark chunk as deleted again
	c->markPtrs(c);

	return first;
}

/**
 * Chunk *mergeFirstChunk(Chunk *c)
 * 
 * Create a new DELETE chunks with M ordered keys taken from all current DELETE chnunks, and buffer.
 * 
 * Parameters:
 * 	@c -pointer to the current first DELETE chunk.
 * 
 * Returned value:
 * 	@c - pointer to the new first DELETE chunk.
 * 
 * **/
Chunk *mergeFirstChunk(Chunk *c)
{
	States state;
	Chunk *merged, *tail;
	Chunk *new, *cur;
	Chunk *next;
	int i, idx;
	int merges_left;
	uint32_t max_key;

	// Unmark 'buffer' and 'next' pointers to copy keys to the new first chunk
	c->markPtrs(c);

	state = DELETE;
	max_key = c->max;

	new = init_chunk(state, max_key);

	merged = new;
	next = c->next;

	for(merges_left = 2; merges_left; merges_left--)
	{
		switch(merges_left)
		{
			case 2: // Copy from current first DELETE chunk
				cur = c;
				break;

			case 1: // Copy from all other current DELETE chunks
				if(next->status.getState(&next->status) == state)
				{
					cur = next;
					next = next->next;
					merges_left++;
				}
				else // Stop copy if we achived first INSERT chunk and try copy keys from buffer
				{
					if(!(cur = c->buffer)) // In case of deleteMin() operation buffer isn't allocated
					{
						continue;
					}
				}
				break;
		}

		idx = new->status.getIdx(&new->status);

		// Copy keys from the current chunk
		for(i = getFrzIdx(cur->status); cur->entries[i] != 0 && i < M && idx < M; i++)
		{
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

				// Find last new DELETE chunk
				while(tail->next)
				{
					tail = tail->next;
				}

				// Add new DELETE chunk to previous DELETE chunk
				tail->next = new;

				idx = new->status.getIdx(&new->status);

				// Continue copy keys from the current chunk
				for(; cur->entries[i] != 0 && i < M && idx < M; i++)
				{
					new->entries[idx] = cur->entries[i];
					new->status.aIncIdx(&new->status);

					idx = new->status.getIdx(&new->status);
				}
			}
		}
	}

	tail = merged;

	// Find last new DELETE chunk
	while(tail->next)
	{
		tail = tail->next;
	}

	// Add first INSERT chunk right after last new DELETE chunk
	tail->next = next;

	tail = merged;

	// Sort keys in each new DELETE chunks
	while(tail->status.getState(&tail->status) == state)
	{
		sort(tail);
		tail = tail->next;
	}

	// Once merge is dode, mark 'buffer' and ''next' pointers as deleted again.
	c->markPtrs(c);

	return merged;
}

/**
 * Chunk *sort(Chunk *c)
 * 
 * Sort keys in chunk in ascending order.
 * 
 * Parameters:
 * 	@c - pointer to chunk to sort keys in.
 * 
 * **/
void sort(Chunk *c)
{
	int i, j;
	uint64_t tmp;

	for(i = 0; c->entries[i] != 0 && i < M; i++)
	{
		for(j = 0; c->entries[j + 1] != 0 && j < M; j++)
		{
			if(c->entries[j] > c->entries[j + 1])
			{
				tmp = c->entries[j];
				c->entries[j] = c->entries[j + 1];
				c->entries[j + 1] = tmp;
			}
		}
	}
}

/**
 * bool is_marked_ref(uintptr_t r)
 * 
 * Check either pointer 'marked' or not.
 * 
 * Parameters:
 * 	@r - pointer to check.
 * 
 * Returned value:
 * 	@true - pointer is 'marked'.
 * 	@false - otherwise.
 * 
 * **/
bool is_marked_ref(uintptr_t r)
{
	return is_marked(r);
}

/**
 * uintptr_t get_marked_ref(uintptr_t r)
 * 
 * Set otherwise-unused low-order bit in pointer as 'marked'.
 * 
 * Parameters:
 * 	@r - pointer to 'mark'.
 * 
 * Returned value:
 * 	@mark(r) - 'marked' pointer.
 * 
 * **/
uintptr_t get_marked_ref(uintptr_t r)
{
	return mark(r);
}

/**
 * uintptr_t get_unmarked_ref(uintptr_t r)
 * 
 * Set otherwise-unused low-order bit in pointer as 'unmarked'.
 * 
 * Parameters:
 * 	@r - pointer to 'unmark'.
 * 
 * Returned value:
 * 	@unmark(r) - 'unmarked' pointer.
 * 
 * **/
uintptr_t get_unmarked_ref(uintptr_t r)
{
	return unmark(r);
}
