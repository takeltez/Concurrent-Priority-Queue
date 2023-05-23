#include "queue.h"

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

	return;
}

void print_queue(Chunk *curbuf)
{
	int i = 0;
	int j;

	Chunk *c = head;

	printf("Buffer:\n\tStatus properties:\n\t\tstate = %u\n\t\tindex = %u\n\t\tfrozenInd = %u\n", 
	       curbuf->status.state, curbuf->status.index, curbuf->status.frozenInd);

	printf("\tChunk properties:\n\t\tmax = %u\n", curbuf->max);

	for(j = 0; curbuf->entries[j] != 0 && j < M; j++)
	{
		printf("\t\tentries[%d] = %lu\n", j, curbuf->entries[j]);
	}

		for (j = 0; curbuf->frozen[j] != 0 && j < M_FROZEN; j++)
		{
			printf("\t\tfrozen[%d] = %lu\n", j, curbuf->frozen[j]);
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

int freezeKeys(Chunk *c)
{
	(void) c;

	return 0;

	/* TODO */
}

int getChunk(Chunk *cur, Chunk *p)
{
	(void) cur;
	(void) p;

	/* TODO */

	return 0;
}

void getChunk_by_key(Chunk *cur, Chunk *prev, int key)
{
	(void) cur;
	(void) prev;
	(void) key;

	/* TODO */
}

void key_CAS(uint64_t *mem, uint64_t old, uint64_t new)
{
	(void) mem;
	(void) old;
	(void) new;

	/* TODO */
}

int chunk_CAS(Chunk **c, Chunk *cur, Chunk *local)
{
	(void) c;
	(void) cur;
	(void) local;

	/* TODO */

	return 0;
}

bool createBuffer(int key, Chunk *cur, Chunk **curbuf)
{
	/* TODO */

	cur->buffer = init_chunk(BUFFER, 20);

	int i;

	for(i = 0; cur->buffer->entries[i] != 0 && i < M; i++);

	cur->buffer->entries[i] = key;

	*curbuf = cur->buffer;

	return true;
}

int getIdx(Status s)
{
	return s.index;
}

Chunk *split(Chunk *cur)
{
	(void) cur;

	/* TODO */

	return cur;
}

Chunk *mergeFirstChunk(Chunk *cur)
{
	(void) cur;

	/* TODO */

	return cur;
}
