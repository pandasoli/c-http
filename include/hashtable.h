#pragma once

#include <stdlib.h>
#include <stdint.h>


typedef unsigned long hashfn(const char *);

typedef struct entry entry;
struct entry {
	char *key;
	const void *val;
	entry *next;
};

typedef struct hashtable hashtable;
struct hashtable {
	hashfn *hash;
	uint32_t size;
	entry **elements;

	void *(*lookup)(hashtable *ht, const char *key);
	void *(*insert)(hashtable *ht, const char *key, size_t key_len, const void *val);
	void (*free)(hashtable ht);
};

hashtable hashtable_create(uint32_t size, hashfn *hf);
unsigned long djb2(const char *key);
