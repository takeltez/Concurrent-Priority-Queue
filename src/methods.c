#include "queue.h"

Status status_aIncIdx(Status *s)
{
	(void) s;

	/* TODO */

	return *s;
}

int status_isInFreeze(Status *s)
{
	return (s->state == FREEZING) ? 1 : 0;
}

int status_getIdx(Status *s)
{
	return s->index;
}

int status_CAS(struct Status *s, struct Status localS, struct Status newS)
{
	(void) s;
	(void) localS;
	(void) newS;

	/* TODO */

	return 0;
}

void status_aOr(struct Status *s, int mask)
{
	(void) s;
	(void) mask;

	/* TODO */
}

void status_set(Status *s, States state, int idx, int frozenInd)
{
	s->state = state;
	s->index = idx;
	s->frozenInd = frozenInd;
}

States status_getState(Status *s)
{
	return s->state;
}

int chunk_entryFrozen(Chunk *c, int idx)
{
	int i;

	for(i = 0; i < M_FROZEN; i ++)
	{
		if(c->frozen[i] == (uint64_t)idx)
		{
			return 1;
		}
	}

	return 0;
}

void chunk_markPtrs(struct Chunk *c)
{
	if(c == NULL)
	{
		return;
	}

	/* If we are about delete first chunck */

	if (c == head)
	{
		head = c->next;

		goto free;
	}

	/* If we are about delete buffer of first chunck */

	if (c == head->buffer)
	{
		head->buffer = NULL;

		goto free;
	}

	/* If we are about delete any other chunck */

	Chunk *prev = head;
	Chunk *next = NULL;

	while(prev->next != c)
	{
		prev = prev->next;
	}

	if((next = prev->next->next))
	{
		prev->next = next;
	}
	else
	{
		prev->next = NULL;
	}

free:
	free(c);
}
