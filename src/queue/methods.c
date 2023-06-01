#include "queue.h"

/**
 * Status status_aIncIdx(Status *s)
 * 
 * Increase status index.
 * 
 * Parameters:
 * 	@s - pointer to Status structure.
 * 
 * Returned value:
 * 	@s - Status structure with new index.
 * 
 * **/
Status status_aIncIdx(Status *s)
{
	__atomic_fetch_add(&s->index, 1, __ATOMIC_RELEASE);

	return *s;
}

/**
 * int status_isInFreeze(Status *s)
 * 
 * Check either current chunk state is FREEZING or not.
 * 
 * Parameters;
 * 	@s - chunk status.
 * 
 * Returned value:
 * 	@true -  current chunk state is FREEZING.
 * 	@false - current chunk state is not FREEZING.
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
 * 	@s - pointer on chunk status.
 * 
 * Returned value:
 * 	@s->index - chunk status index.
 * 
 * **/
int status_getIdx(Status *s)
{
	return s->index;
}

/**
 * bool status_CAS(struct Status *s, struct Status localS, struct Status newS)
 * 
 * Comapre status 's' with status 'localS'.
 * If equal, replace value stored in 's' with value 'newS'.
 * Otherwise, replace value 'localS' with value stored in 's'.
 * 
 * Parameters:
 * 	@s - pointer to Status structure which is supposed to be updated.
 * 	@localS - Status structure which compare 's' with.
 * 	@newS - Status structure which should update 's'.
 * 
 * Returned value:
 * 	@true - if statuses 's' and 'localS' equal.
 * 	@false - otherwise.
 * 
 * **/
bool status_CAS(struct Status *s, struct Status localS, struct Status newS)
{
	return __atomic_compare_exchange_n(&s, &localS, &newS, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
}

/**
 * void status_aOr(struct Status *s, int mask)
 * 
 * Make binary 'or' operation to switch between states.
 * 
 * Parameters:
 * 	@s - pointer to Status structure
 * 	@mask - mask to change state with.
 * 
 * **/
void status_aOr(struct Status *s, int mask)
{
	int state = s->state;

	__atomic_or_fetch(&state, mask, __ATOMIC_RELEASE);
}

/**
 * void status_set(Status *s, States state, int idx, int frozenInd)
 * 
 * Set chunk status fields.
 * 
 * Parameters:
 * 	@s - pointer to chunck status.
 * 	@state - chucnk status state.
 * 	@idx - chunk status index.
 * 	@frozenIdx - chunk status frozen index.
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
 * 	@s - pointer on chunk status.
 * 
 * Returned value:
 * 	@s->state - chunk status state.
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
 * 	@c - pointer to the chunk.
 * 	@idx - index of frozen array.
 * 
 * Returned value:
 * 	@true - if element frozen[idx] contains key.
 * 	@false - if element frozen[idx] doesn't contains key.
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
 * 	@c - pointer to chunk for deletion.
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
		if(c->next)
		{
			head = c->next;
		}
		else
		{
			head = NULL;
		}

		goto free;
	}

	if (c == head->buffer)
	{
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
