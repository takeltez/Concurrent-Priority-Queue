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
 * bool insertToBuffer(int key, Chunk *cur)
 * 
 * Insert key to buffer.
 * 
 * Parameters:
 * @key - key to insert.
 * @cur - pointer to the first chunk.
 * 
 * Returned value:
 * @true - key is placed into buffer.
 * @false - hey is not placed into buffer.
 * 
 * **/
bool insertToBuffer(int key, Chunk *cur)
{
	// PHASE I: key insertion into the buffer
	Chunk *curbuf = cur->buffer;
	bool result = false;

	// the buffer is not yet allocated
	if(curbuf == NULL)
	{ 
		// key added during buffer creation
		if(createBuffer(key, cur, &curbuf))
		{
			goto phaseII;
		}
	}

	// atomically increase the index in the status
	Status s = curbuf->status.aIncIdx(&s);

	int idx = getIdx(s);

	if(idx < M && !s.isInFreeze(&s))
	{
		curbuf->entries[idx] = key;

		#pragma omp barrier

		if(!curbuf->status.isInFreeze(&curbuf->status))
		{
			result = true;
		}
		if(curbuf->entryFrozen(curbuf, idx))
		{
			return true;
		}
	}

phaseII:
	usleep(0);
	freezeChunk(cur);
	freezeRecovery(cur, NULL);

	return result;
}

/**
 * void freezeChunk(Chunk *c)
 *
 * Freeze for a chunk.
 * 
 * Parameters:
 * @c - chunk pointer to freeze.
 * 
 * **/
void freezeChunk(Chunk *c)
{
	int idx, frozenIdx = 0;

	// locally copied status
	Status localS;

	// PHASE I: set the chunk status if needed
	while(1)
	{
		// read the current status to get its state and index
		localS = c->status;
		idx = localS.getIdx(&localS);

		switch(localS.getState(&localS))
		{
			case BUFFER:
			// in insert or buffer chunks frozenIdx was and remained 0

			case INSERT:
				c->status.aOr(&c->status, MASK_FREEZING_STATE);
				break;

			case DELETE:
				if (idx > M)
				{
					frozenIdx = M;
				}
				else
				{
					frozenIdx = idx;
				}

				// set: state, index, frozen index
				Status newS;
				newS.set(&newS, FREEZING, idx, frozenIdx);

				// can fail due to delete updating the index
				if(c->status.CAS(&c->status, localS, newS))
				{
					break;
				}
				else
				{
					continue;
				}

			// in process of being freezed
			case FREEZING:
				break;

			// c was frozen by someone else
			case FROZEN:
				c->markPtrs(c);
				return;
		}
		break; // continue only if CAS from DELETE state failed
	}

	//PHASE II: freeze the entries
	if(c != head)
	{
		freezeKeys(c);
	}

	// from FREEZING to FROZEN using atomic OR
	c->status.aOr(&c->status, MASK_FROZEN_STATE);

	// set the chunk pointers as deleted
	c->markPtrs(c);
}

/**
 * void freezeRecovery(Chunk *cur, Chunk *prev)
 * 
 * Replace the frozen chunk with one or more new chunks that hold the relevant entries of the frozen chunk.
 * 
 * Parameters:
 * @cur - pointer to the chunk which is supposed to be replaced.
 * @prev - pointer to the chunk which should replace frozen chunk.
 * 
 * **/
void freezeRecovery(Chunk *cur, Chunk *prev)
{
	bool toSplit = true;
	Chunk *local = NULL, *p = NULL;

	// PHASE I: decide whether to split or to merge
	while(1)
	{
		if(cur == head || (prev == head && prev->status.isInFreeze(&prev->status)))
		{
			toSplit = false;
		}

		//PHASE II: in split, if prev is frozen, recover it first
		if(toSplit && prev->status.isInFreeze(&prev->status))
		{
			freezeChunk(prev);// ensure prev freeze is done

			// search the previous to prev
			if(getChunk(&prev, &p))
			{
				// the frozen prev found, p precedes prev; recursive recovery
				freezeRecovery(prev, p);
			}

			// prev is already not in the list; re−search the current chunk and find its new predecessor
			if(!getChunk(&cur, &p))
			{
				return; // the frozen cur is not in the list
			}
			else
			{
				prev = p;
				continue;
			}
		}

		// PHASE III: apply the decision locally
		if(toSplit)
		{
			local = split(cur);
		}
		else
		{
			local = mergeFirstChunk(cur);
		}

		// PHASE IV: change the PQ accordingly to the previous decision
		if(toSplit)
		{
			if(chunk_CAS(&prev->next, cur, local))
			{
				return;
			}
		}
		else // when modifying the head, check if cur second or first
		{
			if (prev == NULL)
			{
				if(chunk_CAS(&head, cur, local))
				{
					return;
				}
				else if(chunk_CAS(&head, prev, local))
				{
					return;
				}
			}
		}

		// look for new location; finish if the frozen cur is not found
		if(!getChunk(&cur, &p))
		{
			return;
		}
		else
		{
			prev = p;
		}
	}
}

void freezeKeys(Chunk *c)
{
	(void) c;

	/* TODO */
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
 * void getChunk_by_key(Chunk **cur, Chunk **prev, int key)
 * 
 * Search for a decent chunk in order to store a given key.
 * In case such chunk was found pointer to this chunk stored into *cur, and pointer to previous chunk stored into *prev.
 * 
 * Parameters:
 * 	@cur - double pointer on chunk for store given key.
 * 	@prev - double pointer on previous chunk.
 * 
 * **/
void getChunk_by_key(Chunk **cur, Chunk **prev, int key)
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
 * void key_CAS(uint64_t *mem, uint64_t old, uint64_t new)
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
void key_CAS(uint64_t *mem, uint64_t old, uint64_t new)
{
	__atomic_compare_exchange_n(mem, &old, new, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

/**
 * bool chunk_CAS(Chunk **c, Chunk *cur, Chunk *local)
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
bool chunk_CAS(Chunk **c, Chunk *cur, Chunk *local)
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
		// If buffer has been already allocated by previous thread
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

Chunk *split(Chunk *c)
{
	(void) c;

	/* TODO */

	return c;
}

Chunk *mergeFirstChunk(Chunk *c)
{
	(void) c;

	/* TODO */

	return c;
}
