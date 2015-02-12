#include <string.h>
#include <stdlib.h>

#include "hash.h"


// Hash functions adapted from hashlib hshstrhash and hshstrehash to work
// with any key.

/* Hash a key */
unsigned long hshash(const char * string, size_t N) {

	unsigned long h;

	h = 0;
	while (N--) {
		h = h * 31UL + (unsigned char) *string++;
	}
	return h;
}

/* ReHash any key */
unsigned long hshrehash(const char * string, size_t N) {

	unsigned long h;

	h = 0;
	while (N--) {
		h = h * 37UL + (unsigned char) *string++;
	}
	return h;
}


static void *hitemdupe(void *vitem) {

	hitem_st *item, *src = vitem;
	int lgh;

	item = malloc(sizeof *item);
	if (item) {
		lgh = (sizeof *(item->key)) * src->N;
		item->key = malloc(lgh);
		if (item->key) {
			memcpy(item->key, src->key, lgh);
			item->N = src->N;
			item->ptr = src->ptr;
			item->next = NULL;
		} else {
			free(item);
			item = NULL;
		}
	}
	return item;
}

/* Reverses the action of hwdupe above */
static void hitemundupe(void * vitem) {

	hitem_st *item = vitem;
	free(item->key);
	free(item);
}

/* The hash table only cares about equality      */
/* Reused in sorting, where comparison is needed */
static int hitemcmp(void *litem, void *ritem)
{
	hitem_st *li = litem, *ri = ritem;
	size_t n;

	n = li->N > ri->N ? ri->N : li->N;
	return memcmp(li->key, ri->key, n);
}

static unsigned long hitemhash(void * vitem) {

	hitem_st * hi = vitem;

	return hshash(hi->key, hi->N);
}

static unsigned long hitemrehash(void * vitem) {

	hitem_st * hi = vitem;

	return hshrehash(hi->key, hi->N);
}

/* define a data type for the datum item in tablewalkfn */
typedef struct walkglobals {
	void *previtem;
} walkglobals;

/* This function is called for each item in the database
   during a hashwalk.  It can perform some operation on a
   data item.  It is passed the equivalent of global storage
   (for it) in datum.  It is only used in walking the
   complete stored database. It returns 0 for success.
   xtra will normally be NULL, but may be used for debug
   purposes.  During a database walk, the item parameter
   will never be NULL.  See hshexecfn in hashlib.h
*/
static int tablewalkfn(void *item, void *datum, void *xtra)
{
	walkglobals *global = datum;
	hitem_st    *hitlink = item;

	hitlink->next     = global->previtem;
	global->previtem = hitlink;
	return 0;  /* i.e. no error occured */
} /* tablewalkfn */

/* 1------------------1 */

/* together with tablewalkfn, this uses the 'next' fields
   of the stored items to form a singly linked list */
static hitem_st *formlist(hshtbl *h)
{
	walkglobals globs;

	globs.previtem = NULL;  /* start with end of list */

	/* Now we can scan all active items in the table */
	/* We are saving the err return for completeness */
	/* only.  tablewalkfn() never returns an error   */
	hshwalk(h, tablewalkfn, &globs);

	return globs.previtem;  /* now this is head of list */
} /* formlist */


hash *CreateVoidHash() {
	hash *newHash = malloc(sizeof(*newHash));
	newHash->first_in_loop = NULL;
	newHash->next_in_loop = NULL;
	newHash->htable = hshinit(hitemhash, hitemrehash,
							  hitemcmp,
							  hitemdupe, hitemundupe, 0);
	if (newHash->htable == NULL) {
		free(newHash);
		newHash = NULL;
	}
	return newHash;
}

void DestroyHash(hash **theHash) {
	hash *thisHash = *theHash;
	if (! thisHash) return;
	if (thisHash->htable) {
		hshkill(thisHash->htable);
	}
	free(thisHash);
	*theHash = NULL;
}

void *InsertHashFreeElement(hash *theHash, char *theKey,
							size_t N, void *item) {

	static hitem_st hitem;
	hitem_st* hitem_store;

	if (!theHash) return NULL;

	theHash->first_in_loop = NULL;
	theHash->next_in_loop = NULL;

	hitem.key = theKey;
	hitem.N = N;
	hitem.ptr = item;
	hitem_store = hshfind(theHash->htable, &hitem);
	if (hitem_store) {
		hitem_store->ptr = item;
	} else {
		hitem_store = hshinsert(theHash->htable, &hitem);
	}
	if (!hitem_store) return NULL;
	return hitem_store->ptr;
}


void *InsertHashElement(hash *theHash, char *theKey, void *item) {

	return InsertHashFreeElement(theHash, theKey, strlen(theKey) + 1, item);
}

void *FindHashFreeElement(hash *theHash, char *theKey, size_t N) {

	hitem_st* hitem_find;
	hitem_st hitem;

	if (!theHash) return NULL;

	hitem.key = theKey;
	hitem.N   = N;
	hitem.ptr = NULL;

	hitem_find = hshfind(theHash->htable, &hitem);

	if (!hitem_find) return NULL;
	return hitem_find->ptr;

}

void *FindHashElement(hash *theHash, char *theKey) {

	return FindHashFreeElement(theHash, theKey, strlen(theKey) + 1);
}

void *FindOrInsertHashFreeElement(hash *theHash, char *theKey,
								  size_t N, void *item) {

	hitem_st hitem;
	hitem_st* hitem_store;

	if (!theHash) return NULL;

	theHash->first_in_loop = NULL;
	theHash->next_in_loop = NULL;

	hitem.key = theKey;
	hitem.N = N;
	hitem.ptr = item;
	hitem_store = hshinsert(theHash->htable, &hitem);
	if (!hitem_store) return NULL;
	return hitem_store->ptr;
}

void *FindOrInsertHashElement(hash *theHash, char *theKey, void *item) {

	return FindOrInsertHashFreeElement(theHash, theKey, strlen(theKey) + 1, item);

}

int RemoveHashFreeElement(hash *theHash, char *theKey,
						  size_t N) {

	hitem_st hitem;
	hitem_st* hitem_rm;
	int ret_code = 0;

	if (!theHash) return 0;

	hitem.key = theKey;
	hitem.N   = N;
	hitem.ptr = NULL;

	hitem_rm = hshdelete(theHash->htable, &hitem);
	if (hitem_rm) {
		hitemundupe(hitem_rm);
		ret_code = 1;
	}
	return ret_code;
}

int RemoveHashElement(hash *theHash, char *theKey) {
	return RemoveHashFreeElement(theHash, theKey, strlen(theKey) + 1);
}


void *StartLoopHash( hash *theHash ) {

	hitem_st *first;

	if (!theHash) return NULL;

	if (!theHash->first_in_loop) {
		theHash->first_in_loop = formlist(theHash->htable);
	}
	first = theHash->first_in_loop;

	if (!first) return NULL; // empty hash

	theHash->next_in_loop = first->next;
	return first->ptr;
}

void *GetNextHash( hash *theHash ) {

	hitem_st *next;
	hitem_st *result;

	if (!theHash) return NULL;
	if (theHash->next_in_loop == NULL) {
		next = NULL;
		result = NULL;
	} else {
		next = theHash->next_in_loop;
		if (next == NULL) {
			theHash->next_in_loop = NULL;
			result = NULL;
		} else {
			theHash->next_in_loop = next->next;
			result = next->ptr;
		}
	}
	return result;
}
