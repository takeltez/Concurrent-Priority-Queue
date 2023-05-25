#include "queue.h"

/**
 * Chunk *init_chunk(States state, uint32_t max)
 * 
 * Initialize new chunk fields.
 * 
 * Parameters:
 * 	state - chunk state.
 * 	max - chunk maximal possible stored key.
 * 
 * Returned value:
 * 	c - pointer to the new chunk.
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
 * Create new chunk
 * 
 * Parameters:
 * max - chunk maximal possible stored key.
 * 
 * **/
void create_chunk(uint32_t max)
{
	if (head == NULL)
	{
		Chunk *c = init_chunk(DELETE, max);

		head = c;

		return;
	}

	Chunk *tail = head;

	while (tail->next)
	{
		tail = tail->next;
	}

	Chunk *c = init_chunk(INSERT, max);

	tail->next = c;
}

/**
 * void print_queue(Chunk *root)
 * 
 * Print queue state
 * 
 * Parameters:
 * 	root - pointer to queue head
 * 
 * **/
void print_queue(Chunk *root)
{
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

bool instertToBuffer(int key, Chunk *cur, Chunk *curhead)
{
	(void) key;
	(void) cur;
	(void) curhead;

	/* TODO */

	return true;
}

void freezeCunck(Chunk *c)
{
	(void) c;

	/* TODO */
}

void freezeRecovery(Chunk *cur, Chunk *prev)
{
	(void) cur;
	(void) prev;

	/* TODO */
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
 * 	cur - double pointer to required chunk.
 * 	prev - double pointer to previous chunk.
 * 
 * Returned value:
 * 	true - in case chunk *cur was found in chunk list.
 * 	false - in case chunk *cur was not found in chunk list.
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
		if(c->next == *cur && (*cur)->status.state == FROZEN)
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
 * 	cur - double pointer on chunk for store given key.
 * 	prev - double pointer on previous chunk.
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

void key_CAS(uint64_t *mem, uint64_t old, uint64_t new)
{
	(void) mem;
	(void) old;
	(void) new;

	/* TODO */
}

bool chunk_CAS(Chunk **c, Chunk *cur, Chunk *local)
{
	(void) c;
	(void) cur;
	(void) local;

	/* TODO */

	return true;
}

/**
 * bool createBuffer(int key, Chunk *c, Chunk **buf)
 * 
 * Create buffer for the first chunk, add key into this buffer and store pointer to this buffer into *buf.
 * 
 * Parameters:
 * 	key - key to be added into buffer.
 * 	c - pointer on the first chunk.
 * 	buf -double pointer to the buffer
 * 
 * Returned value:
 * 	true - the new buffer was successfully connected to the first chunk.
 * 	false - another thread had connected another buffer.
 * 
 * **/
bool createBuffer(int key, Chunk *c, Chunk **buf)
{
	/**
	 * As follows from description this function still isn't finished.
	 * Need to figure out the case for return false value.
	 * 
	 * **/

	c->buffer = init_chunk(BUFFER, 20);

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
 * 	s - chunk status.
 * 
 * Returned value:
 * 	s.index - chunk status index.
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
