#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include "hashlib.h"

typedef struct _hitem_st {
	char *key;
	size_t N;                // lenght of key in bytes
	void *ptr;
	struct _hitem_st *next;
} hitem_st;


typedef struct _hash {
	hshtbl * htable;
	hitem_st *first_in_loop;
	hitem_st *next_in_loop;
} hash;

/**
 * Create an empty hash.
 *
 *
 * @return a newly created empty hash.
 */

hash *CreateVoidHash();

/**
 * Destroy the hash. Before destroying the hash you should perhaps destroy
 * the elements the hash points to (if elements are owned by the hash). For
 * instance, for a hash of textures:
 *
 * Texture *elem;
 *  for( item = StartLoopHash(tex_hash); item; item = GetNextHash(tex_hash) )
 *    DestroyTexture(&item);
 *
 * @param theHash a pointer to the hash structure.
 */

void DestroyHash(hash **theHash);

/**
 * Insert an element into the hash. The key is a string. If an element with
 * the same key is in the hash, replace it.
 *
 * @param theHash the hash structure
 * @param theKey the key string
 * @param item pointer to the item to be inserted
 *
 * @return a pointer to the inserted element in the hash or NULL if an error
 * occurred.
 */

void *InsertHashElement(hash *theHash, char *theKey, void *item);

/**
 * Insert an element into the hash. The key can be anything. If an element
 * with the same key is in the hash, replace it.
 *
 * @param theHash the hash structure
 * @param theKey pointer to the key
 * @param N key lenght
 * @param item pointer to the item to be inserted
 *
 * @return a pointer to the inserted element in the hash or NULL if an
 * error occurred.
 */

void *InsertHashFreeElement(hash *theHash, char *theKey, size_t N, void *item);


/**
 * Find an element in the hash. The key is a string.
 *
 * @param theHash the hash structure
 * @param theKey the key string
 *
 * @return a pointer to the element or NULL if not present.
 */

void *FindHashElement(hash *theHash, char *theKey);

/**
 * Find an element in the hash. The key can be anything
 *
 * @param theHash the hash structure
 * @param theKey pointer to the key
 * @param N key lenght
 *
 * @return a pointer to the element or NULL if not present.
 */

void *FindHashFreeElement(hash *theHash, char *theKey, size_t N);


/**
 * Find an element in the hash. If not there, insert a new element. The key
 * is a string.
 *
 * @param theHash the hash structure
 * @param theKey the key string
 * @param item pointer to the item to be inserted (if key is not in hash).
 *
 * @return a pointer to the element or NULL if error.
 */

void *FindOrInsertHashElement(hash *theHash, char *theKey, void *item);

/**
 * Find an element in the hash. If not there, insert a new element. The key
 * can be anything.
 *
 * @param theHash the hash structure
 * @param theKey pointer to the key
 * @param theKey key lenght
 *
 * @param item pointer to the item to be inserted In case key is not in hash.
 *
 * @return a pointer to the element or NULL if error.
 */

void *FindOrInsertHashFreeElement(hash *theHash, char *theKey, size_t N, void *item);


/**
 * Remove a hash element
 *
 * @param theHash the hash structure
 * @param theKey the key string
 *
 * @return 1 if success, 0 if not.
 */

int RemoveHashElement(hash *theHash, char *theKey);

/**
 * Remove a hash element
 *
 * @param theHash the hash structure
 * @param theKey pointer to the key
 * @param N key lenght
 *
 * @return 1 if success, 0 if not.
 */

int RemoveHashFreeElement(hash *theHash, char *theKey, size_t N);

/**
 * Start a loop for iterating the hash. The order of elements is not specified.
 *
 * @param theHash the hash structure
 *
 * @return pointer to the first element
 */
void *StartLoopHash( hash *theHash );

/**
 * Get the next element of the hash (order is not specified). theHash must
 * point to a valid hash (initialized with StartHashLoop)
 *
 * @param theHash the hash structure. It must point to a valid hash (initialized with StartHashLoop)
 *
 * @return the next element in the loop.
 */

void *GetNextHash( hash *theHash );

#endif
