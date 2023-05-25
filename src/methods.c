#include "queue.h"

Status status_aIncIdx(Status *s)
{
	(void) s;

	/* TODO */

	return *s;
}

/**
 * int status_isInFreeze(Status *s)
 * 
 * Check either current chunk state is FREEZING or not.
 * 
 * Parameters;
 * 	s - chunk status.
 * 
 * Returned value:
 * 	true -  current chunk state is FREEZING.
 * 	false - current chunk state is not FREEZING.
 * 
 * **/
bool status_isInFreeze(Status *s)
{
	return (s->state == FREEZING) ? true : false;
}

/**
 * int status_getIdx(Status *s)
 * 
 * Get chunk status index.
 * 
 * Parameters:
 * 	s - pointer on chunk status.
 * 
 * Returned value:
 * 	s->index - chunk status index.
 * 
 * **/
int status_getIdx(Status *s)
{
	return s->index;
}

bool status_CAS(struct Status *s, struct Status localS, struct Status newS)
{
	(void) s;
	(void) localS;
	(void) newS;

	/* TODO */

	return true;
}

void status_aOr(struct Status *s, int mask)
{
	(void) s;
	(void) mask;

	/* TODO */
}

/**
 * void status_set(Status *s, States state, int idx, int frozenInd)
 * 
 * Set chunk status fields.
 * 
 * Parameters:
 * 	s - pointer to chunck status.
 * 	state - chucnk status state.
 * 	idx - chunk status index.
 * 	frozenIdx - chunk status frozen index.
 * 
 * **/
void status_set(Status *s, States state, int idx, int frozenInd)
{
	s->state = state;
	s->index = idx;
	s->frozenInd = frozenInd;
}

/**
 * States status_getState(Status *s)
 * 
 * Get chunk status state.
 * 
 * Parameters:
 * 	s - pointer on chunk status.
 * 
 * Returned value:
 * 	s->state - chunk status state.
 * 
 * **/
States status_getState(Status *s)
{
	return s->state;
}

/**
 * bool chunk_entryFrozen(Chunk *c, int idx)
 * 
 * Check either chunk key is frozen or not.
 * 
 * Paramerers:
 * 	c - pointer to the chunk
 * 	idx - index of frozen array.
 * 
 * Returned value:
 * 	true - if element frozen[idx] contains key.
 * 	false - if element frozen[idx] doesn't contains key.
 * 
 * **/
bool chunk_entryFrozen(Chunk *c, int idx)
{
	if(c->frozen[idx] != 0)
	{
		return true;
	}

	return false;
}

/**
 * void chunk_markPtrs(struct Chunk *c)
 * 
 * Delete chunk from queue.
 * 
 * Parameters:
 * 	c - pointer to chunk for deletion.
 * 
 * **/
void chunk_markPtrs(struct Chunk *c)
{
	if(c == NULL)
	{
		return;
	}

	if (c == head)
	{
		head = c->next;

		goto free;
	}

	if (c == head->buffer)
	{
		head->buffer = NULL;

		goto free;
	}

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
