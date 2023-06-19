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

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

/* Chunk capacity */

#define M 928
#define VALS_PER_WORD 63
#define M_FROZEN (M / VALS_PER_WORD + 1)

/* Number of chunks */

#define MAX_CHUNKS 3

/* First chunk maximal key */

#define FIRST_CHUNK_MAX_KEY 20

/* Masks for switching chunk states */

#define MASK_FREEZING_STATE 3
#define MASK_FROZEN_STATE 7

/* Empty enty identifier */

#define EMPTY_ENTRY 0

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
	int (*getKey)(struct Chunk *c, int idx);
}Chunk;

/* Global bointer to the first chunk */

extern Chunk *head;

/* Main functions */

void insert(int key, int val);
int deleteMin(void);

bool insertToBuffer(int key, int val, Chunk *cur);
bool createBuffer(int key, int val, Chunk *c, Chunk **curbuf);

void freezeChunk(Chunk *c);
void freezeRecovery(Chunk *cur, Chunk *prev);

void freezeKeys(Chunk *c);

/* Auxiliary functions */

void create_queue(void);
void destroy_queue(void);

Status init_status(States state);
Chunk *init_chunk(States state, uint32_t max);
void create_chunk(uint32_t max);

bool getChunk(Chunk **cur, Chunk **prev);
void getChunkByKey(Chunk **cur, Chunk **prev, int key);

void keyCAS(uint64_t *mem, uint64_t old, uint64_t new);
bool chunkCAS(Chunk **c, Chunk *cur, Chunk *local);

int getIdx(Status s);
int getFrzIdx(Status s);

Chunk *split(Chunk *cur);
Chunk *mergeFirstChunk(Chunk *cur);

void sort(Chunk *c);

uint64_t key_val_encode(uint64_t key, uint64_t val);
void key_val_decode(uint64_t keyVal, uint64_t *key, uint64_t *val);

/* Status's methods */

Status status_aIncIdx(Status *s);
Status status_aIncFrzIdx(Status *s);
bool status_isInFreeze(Status *s);
int status_getIdx(Status *s);
bool status_CAS(Status *s, Status localS, Status newS);
void status_aOr(Status *s, int mask);
void status_aXor(Status *s, int mask);
void status_set(Status *s, States state, int idx, int frozenInd);
States status_getState(Status *s);

/* Chunk's methods */

bool chunk_entryFrozen(Chunk *c, int idx);
int chunk_getKey(Chunk *c, int idx);

#endif
