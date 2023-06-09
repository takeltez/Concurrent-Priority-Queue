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
 * Status status_aIncFrzIdx(Status *s)
 * 
 * Increase status frozen index.
 * 
 * Parameters:
 * 	@s - pointer to Status structure.
 * 
 * Returned value:
 * 	@s - Status structure with new frozen index.
 * 
 * **/
Status status_aIncFrzIdx(Status *s)
{
	int frozenInd = s->frozenInd;

	__atomic_fetch_add(&frozenInd, 1, __ATOMIC_RELEASE);

	s->frozenInd = frozenInd;

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
	int cur_state;
	int exp_state;

	cur_state = s->state;
	exp_state = FREEZING;

	return __atomic_compare_exchange_n(&cur_state, &exp_state, exp_state, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
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
bool status_CAS(Status *s, Status localS, Status newS)
{
	bool res;

	res = __atomic_compare_exchange_n((Status **)s, &localS, &newS, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);

	#pragma omp critical
	{
		if(res)
		{
			s->set(s, newS.state, newS.index, newS.frozenInd);
		}
	}

	return res;
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
void status_aOr(Status *s, int mask)
{
	int state;

	state = s->state;

	__atomic_or_fetch(&state, mask, __ATOMIC_RELEASE);

	s->state = state;
}

/**
 * void status_aXor(struct Status *s, int mask)
 * 
 * Make binary 'xor' operation to switch between states.
 * 
 * Parameters:
 *  @s - pointer to Status structure
 *  @mask - mask to change state with.
 * 
 * **/
void status_aXor(Status *s, int mask)
{
	int state;

	state = s->state;

	__atomic_xor_fetch(&state, mask, __ATOMIC_RELEASE);

	s->state = state;
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
 * 	@idx - index of key.
 * 
 * Returned value:
 * 	@true - if key is already marked as frozen.
 * 	@false - otherwise.
 * 
 * **/
bool chunk_entryFrozen(Chunk *c, int idx)
{
	int i, k = 0;
	int last_frz_idx = 0;
	uint64_t freezeWord;

	// Find frozen array index
	for(i = 0; i < idx; i++)
	{
		if(!(i % VALS_PER_WORD) && i)
		{
			k++;
		}
	}

	// Set freezeWord
	freezeWord = c->frozen[k];

	// Count number of 1 bits in freezeWord using Brian-Kernighan technique
	while(freezeWord != 0)
	{
		freezeWord = freezeWord & (freezeWord - 1);

		last_frz_idx++;
	}

	// Find index of last frozen key in scope of current frozen array element
	last_frz_idx--;

	// Find absolute index of last frozen key
	if((last_frz_idx + k * VALS_PER_WORD) >= idx)
	{
		// If absolute index of last frozen key is greater or equal than checked index then key is already frozen
		return true;
	}

	// Cheked key is not frozen
	return false;
}

/**
 * void chunk_markPtrs(struct Chunk *c)
 * 
 * Mark 'buffer' and 'next' chunk pointers using the common Harris delete-bit technique.
 * 
 * Parameters:
 * 	@c - pointer to chunk for mark pointers in.
 * 
 * **/
void chunk_markPtrs(Chunk *c)
{
	uintptr_t ref_buff, ref_next;

	ref_buff = (uintptr_t)(void*) c->buffer;
	ref_next = (uintptr_t)(void*) c->next;

	if(c->buffer)
	{
		if(is_marked_ref(ref_buff))
		{
			__atomic_compare_exchange_n(&c->buffer, &ref_buff, get_unmarked_ref(ref_buff), false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
		}
		else
		{
			__atomic_compare_exchange_n(&c->buffer, &ref_buff, get_marked_ref(ref_buff), false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
		}
	}

	if(c->next)
	{
		if(is_marked_ref(ref_next))
		{
			__atomic_compare_exchange_n(&c->next, &ref_next, get_unmarked_ref(ref_next), false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
		}
		else
		{
			__atomic_compare_exchange_n(&c->next, &ref_next, get_marked_ref(ref_next), true, __ATOMIC_RELEASE, __ATOMIC_RELAXED);
		}
	}
}

/**
 * int chunk_getKey(Chunk *c, int idx)
 * 
 * Get key from 64-bit word.
 * 
 * Parameters:
 * 	@c - pointer to the chunk contains desired key.
 * 	@idx - index of desired key.
 * 
 * Returned value:
 * 	@c->entries[idx] >> 32 - desired key.
 * 
 * **/
int chunk_getKey(Chunk *c, int idx)
{
	return c->entries[idx] >> 32;
}
