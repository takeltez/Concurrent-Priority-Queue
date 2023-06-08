#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>
#include <omp.h>

/* Chunk capacity */

#define M 928
#define VALS_PER_WORD 63
#define M_FROZEN (M / VALS_PER_WORD + 2)

/* Number of chunks */

#define MAX_CHUNKS 3

/* Masks for switching chunk states */

#define MASK_FREEZING_STATE 3
#define MASK_FROZEN_STATE 7

/* Empty enty identifier */

#define EMPTY_ENTRY 0

/* Mark pointer masks */

#define mark(x) ((x) | 1)
#define unmark(x) (((x) | 1) ^ 1)
#define is_marked(x) (!!((x) & 1))

/* Chunk states */

typedef enum
{
	INSERT,
	DELETE,
	BUFFER,
	FREEZING,
	FROZEN
} States;

/* Status structure */

typedef struct Status
{
	States state:3;
	uint32_t frozenInd:29;
	uint32_t index;

	struct Status (*aIncIdx)(struct Status *s);
	struct Status (*aIncFrzIdx)(struct Status *s);
	bool (*isInFreeze)(struct Status *s);
	int (*getIdx)(struct Status *s);
	bool (*CAS)(struct Status *s, struct Status localS, struct Status newS);
	void (*aOr)(struct Status *s, int mask);
	void (*aXor)(struct Status *s, int mask);
	void (*set)(struct Status *s, States state, int idx, int frozenInd);
	States (*getState)(struct Status *s);
} Status;

/* Chunk structure */

typedef struct Chunk
{
	uint32_t max;
	uint64_t entries[M];
	uint64_t frozen[M_FROZEN];
	struct Status status;
	struct Chunk *next, *buffer;

	bool (*entryFrozen)(struct Chunk *c, int idx);
	void (*markPtrs)(struct Chunk *c);
}Chunk;

/* Global bointer to the first chunk */

extern Chunk *head;

/* Main functions */

void insert(int key);
int deleteMin(void);

bool insertToBuffer(int key, Chunk *cur);
void freezeChunk(Chunk *c);
void freezeRecovery(Chunk *cur, Chunk *prev);
void freezeKeys(Chunk *c);

/* Auxiliary functions */

void create_queue(void);
void destroy_queue(void);
void print_queue(Chunk *root);

Status init_status(States state);
Chunk *init_chunk(States state, uint32_t max);
void create_chunk(uint32_t max);

bool getChunk(Chunk **cur, Chunk **prev);
void getChunkByKey(Chunk **cur, Chunk **prev, int key);

void keyCAS(uint64_t *mem, uint64_t old, uint64_t new);
bool chunkCAS(Chunk **c, Chunk *cur, Chunk *local);

bool createBuffer(int key, Chunk *c, Chunk **curbuf);
int getIdx(Status s);
int getFrzIdx(Status s);

Chunk *split(Chunk *cur);
Chunk *mergeFirstChunk(Chunk *cur);

void sort(Chunk *c);

bool is_marked_ref(uintptr_t p);
uintptr_t get_marked_ref(uintptr_t p);
uintptr_t get_unmarked_ref(uintptr_t p);

/* Status's methods */

Status status_aIncIdx(Status *s);
Status status_aIncFrzIdx(Status *s);
bool status_isInFreeze(Status *s);
int status_getIdx(Status *s);
bool status_CAS(struct Status *s, struct Status localS, struct Status newS);
void status_aOr(struct Status *s, int mask);
void status_aXor(struct Status *s, int mask);
void status_set(Status *s, States state, int idx, int frozenInd);
States status_getState(Status *s);

/* Chunk's methods */

bool chunk_entryFrozen(Chunk *c, int idx);
void chunk_markPtrs(struct Chunk *c);

#endif
