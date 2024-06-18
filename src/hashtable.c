#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "../include/hashtable.h"


static size_t index_(hashtable *ht, const char *key) {
	return ht->hash(key) % ht->size;
}

static void *lookup(hashtable *ht, const char *key) {
	assert(ht != NULL);
	assert(key != NULL);

	unsigned long i = index_(ht, key);

	entry *e = ht->elements[i];

	while (e != NULL && strcmp(e->key, key) != 0)
		e = e->next;

	if (e == NULL) return NULL;
	return (void *) e->val;
}

static void *insert(hashtable *ht, const char *key, size_t key_len, const void *val) {
	assert(ht != NULL);
	assert(key != NULL);
	assert(val != NULL);

	entry *e = malloc(sizeof(*e));

	char *nkey = malloc(key_len);
	strncpy(nkey, key, key_len);
	nkey[key_len] = 0;

	e->key = nkey;
	e->val = val;

	unsigned long i = index_(ht, key);
	e->next = ht->elements[i];
	ht->elements[i] = e;

	return (void *) val;
}

static void free_(hashtable ht) {
	for (int i = 0; i < ht.size; ++i) {
		entry *e = ht.elements[i];

		while (e != NULL) {
			free(e->key);
			e = e->next;
		}
	}
}

hashtable hashtable_create(uint32_t size, hashfn *hf) {
	hashtable ht;
	ht.hash = hf;
	ht.size = size;
	ht.elements = calloc(sizeof(entry *), ht.size);

	ht.lookup = &lookup;
	ht.insert = &insert;
	ht.free = &free_;

	return ht;
}

unsigned long djb2(const char *key) {
	unsigned long hash = 5381;
	int c;

	while ((c = *key++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}
