#include "queue.h"

Chunk *head = NULL;

/**
 * void insert(int key)
 * 
 * Insert key/value pair into queue.
 * 
 * Parameters:
 * 	@key - key to insert.
 * 
 * **/
void insert(int key, int val)
{
	// Create pointers to current and previous chunks
	Chunk *cur = NULL;
	Chunk *prev = NULL;
	Status s;
	int idx;

	// All threads start at the same time
	#pragma omp barrier

	while(1)
	{
		// Set the current and previous chunk pointers
		getChunkByKey(&cur, &prev, key);

		// If it is the first chunk, insert in the buffer instead
		if(cur == head)
		{
			if(insertToBuffer(key, val, cur))
			{
				return;
			}
			else
			{
				continue;
			}
		}

		s = cur->status;
		idx = getIdx(s);
		
		// atomically increase the index in the status
		cur->status.aIncIdx(&cur->status);

		// insert into a non-full and non-frozen chunk
		if(idx < M && !s.isInFreeze(&s))
		{
			cur->entries[idx] = key_val_encode(key, val);

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
 * Delete key/value pair with higher priority from queue.
 * 
 * Returned value:
 * 	@cur->entries[idx] - deleted key/value pair.
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
		s = cur->status;

		idx = getFrzIdx(s);

		// atomically increase the frozenInd in the status
		cur->status.aIncFrzIdx(&cur->status);

		if(idx < M && !s.isInFreeze(&s)) // delete from not full and non≠frozen chunk
		{
			return cur->entries[idx];
		}

		freezeChunk(cur);
		freezeRecovery(cur, NULL); // Freeze, then restructure the CBPQ and retry
	}

	return 0;
}

/**
 * bool insertToBuffer(int key, Chunk *cur)
 * 
 * Insert key/value pair to buffer.
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
bool insertToBuffer(int key, int val, Chunk *cur)
{
	// PHASE I: key insertion into the buffer
	Chunk *curbuf = cur->buffer;
	Status s;
	int idx;
	bool result = false;

	// the buffer is not yet allocated
	if(curbuf == NULL)
	{
		// key added during buffer creation
		if(createBuffer(key, val, cur, &curbuf))
		{
			result = true;
			goto phaseII;
		}
	}

	s = curbuf->status;
	idx = getIdx(s);

	// atomically increase the index in the status
	curbuf->status.aIncIdx(&curbuf->status);

	if(idx < M && !s.isInFreeze(&s))
	{
		curbuf->entries[idx] = key_val_encode(key, val);

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

phaseII: // PHASE II: first chunk merges with buffer before insert ends
	usleep(0); // yield, give other threads a chance
	freezeChunk(cur);
	freezeRecovery(cur, NULL);

	return result;
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
bool createBuffer(int key, int val, Chunk *c, Chunk **curbuf)
{
	Chunk *buf, *null_ptr;
	bool res;

	null_ptr = NULL;

	buf = init_chunk(BUFFER, 20);

	buf->entries[0] = key_val_encode(key, val); // buffer is created with the key

	res = __atomic_compare_exchange_n(&c->buffer, &null_ptr, buf, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED);

	*curbuf = buf; // update buffer ptr (ours or someone’s else)

	return res;
}

/**
 * void freezeChunk(Chunk *c)
 *
 * Prepare chunk for freeze.
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
			case BUFFER: // in insert or buffer chunks frozenIdx was and remained 0
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
					frozenIdx = getFrzIdx(c->status);
				}

				Status newS;
				newS = init_status(localS.getState(&localS));

				// set: state, index, frozen index
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

	// from FREEZING to FROZEN using atomic XOR
	c->status.aXor(&c->status, MASK_FROZEN_STATE);

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
	bool isInFreeze = false;
	Chunk *local = NULL, *p = NULL;

	if(prev)
	{
		isInFreeze = prev->status.isInFreeze(&prev->status);
	}

	// PHASE I: decide whether to split or to merge
	while(1)
	{
		if(cur == head || (prev == head && isInFreeze))
		{
			toSplit = false;
		}
		//PHASE II: in split, if prev is frozen, recover it first
		if(toSplit && isInFreeze)
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
			if(chunkCAS(&prev->next, cur, local))
			{
				return;
			}
		}
		else // when modifying the head, check if cur second or first
		{
			if (prev == NULL)
			{
				if(chunkCAS(&head, cur, local))
				{
					return;
				}
			}
			else if(chunkCAS(&head, prev, local))
			{
				return;
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

/**
 * void freezeKeys(Chunk *c)
 * 
 * Set words in the frozen array.
 * 
 * Parameters:
 * 	@c - pointer to the chunk.
 * 
 * **/
void freezeKeys(Chunk *c)
{
	int k, i, entry, idx;
	uint64_t freezeWord, mask;
	uint64_t l_val, r_val;

	// go over entries which are held by one freeze word
	for(k = 0; k < M / VALS_PER_WORD + 1; k++)
	{
		l_val = 1, r_val = 64;

		// 0's with MSB set to 1
		freezeWord = l_val << r_val;
		mask = 1;

		// prepare a mask for one frozen word
		// VALS_PER_WORD is 63
		for(i = 0; i < VALS_PER_WORD; i++, mask <<= 1)
		{
			if((idx = i + k * VALS_PER_WORD) == M)
			{
				break;
			}

			//read the entry
			entry = c->entries[idx];

			if(entry != EMPTY_ENTRY)
			{
				freezeWord |= mask;
			}
		} // end of preparing mask for a single frozen world

		// try to update the frozen array
		// after this CAS, surely MSB of the frozen array is set
		keyCAS(&c->frozen[k], 0, freezeWord);
	}
}
