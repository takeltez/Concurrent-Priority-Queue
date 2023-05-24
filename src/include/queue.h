#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>

#define M 928
#define M_FROZEN (M / 63 + 1)
#define MAX_CHUNKS 3

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
	int (*isInFreeze)(struct Status *s);
	int (*getIdx)(struct Status *s);
	int (*CAS)(struct Status *s, struct Status localS, struct Status newS);
	void (*aOr)(struct Status *s, int mask);
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

	int (*entryFrozen)(struct Chunk *c, int idx);
	void (*markPtrs)(struct Chunk *c);
}Chunk;

extern Chunk *head;

/* Main functions */

void instert(int key);
int deleteMin(void);

/* Auxiliary functions */

Chunk *init_chunk(States state, uint32_t max);
void create_chunk(uint32_t max);
void print_queue(Chunk *curbuf);

bool instertToBuffer(int key, Chunk *cur, Chunk *curhead);
void freezeCunck(Chunk *c);
void freezeRecovery(Chunk *cur, Chunk *prev);
int freezeKeys(Chunk *c);

int getChunk(Chunk *cur, Chunk *p);
void getChunk_by_key(Chunk *cur, Chunk *prev, int key);

void key_CAS(uint64_t *mem, uint64_t old, uint64_t new);
int chunk_CAS(Chunk **c, Chunk *cur, Chunk *local);

bool createBuffer(int key, Chunk *cur, Chunk **curbuf);
int getIdx(Status s);

Chunk *split(Chunk *cur);
Chunk *mergeFirstChunk(Chunk *cur);

/* Status's methods */

Status status_aIncIdx(Status *s);
int status_isInFreeze(Status *s);
int status_getIdx(Status *s);
int status_CAS(struct Status *s, struct Status localS, struct Status newS);
void status_aOr(struct Status *s, int mask);
void status_set(Status *s, States state, int idx, int frozenInd);
States status_getState(Status *s);

/* Chunk's methods */

int chunk_entryFrozen(Chunk *c, int idx);
void chunk_markPtrs(struct Chunk *c);

#endif
