#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

listElement *CreateListElement( void * dataPointer ) {

	listElement *firstListElement;

	firstListElement = malloc( sizeof(*firstListElement) * 1 );
	firstListElement->previous      = NULL;
	firstListElement->next          = NULL;
	firstListElement->pointer       = dataPointer;

	return firstListElement;
}

void DestroyListElement( listElement *thisElement ) {

	listElement *ll;
	ll = thisElement->next;
	if( ll ) {
		ll->previous = thisElement->previous;
	}
	ll = thisElement->previous;
	if( ll ) {
		ll->next = thisElement->next;
	}

	free( thisElement );
}

listElement *AddBeforeElement( listElement *thisElement, void *dataPointer ) {

	listElement *newListElement;
	listElement *tmp;

	if( thisElement->pointer) {
		newListElement = malloc( sizeof(*newListElement) * 1 );
		tmp                      = thisElement->previous;
		newListElement->next     = thisElement;
		newListElement->previous = tmp;
		newListElement->pointer  = dataPointer;
		thisElement->previous    = newListElement;
		if( tmp ) {
			tmp->next              = newListElement;
		}
	}
	else {
		/* if this list is empty, insert the pointer*/
		thisElement->pointer    = dataPointer;
		newListElement          = thisElement;
	}
	return newListElement;
}

listElement *AddAfterElement( listElement *thisElement, void *dataPointer ) {

	listElement *newListElement;
	listElement *tmp;

	if( thisElement->pointer) {
		newListElement = malloc( sizeof(*newListElement) * 1 );
		newListElement->previous = thisElement;
		newListElement->next     = thisElement->next;
		newListElement->pointer  = dataPointer;
		tmp                      = thisElement->next;
		thisElement->next        = newListElement;
		if( tmp ) {
			tmp->previous          = newListElement;
		}
	}
	else {
		/* if this list is empty, insert the pointer*/
		thisElement->pointer = dataPointer;
		newListElement       = thisElement;
	}
	return newListElement;
}

// ================================================================

list *CreateVoidList() {

	list  *newList;

	newList = malloc( sizeof(*newList) * 1 );
	newList->N              = 0;
	newList->first          = NULL;
	newList->last           = NULL;
	newList->nextInLoop     = NULL;
	return newList;
}

void  DestroyList( list **theList ) {

	list            *thisList = *theList;
	if( ! thisList ) return;

	EmptyList( thisList );
	free( thisList );
	*theList = NULL;
}

void  EmptyList( list *thisList ) {

	listElement *ll1;
	listElement *ll2;

	if( ! thisList ) return;

	if( thisList->N ) {
		ll1 = thisList->first;
		while( ll1 ) {
			ll2 = ll1;
			ll1 = ll1->next;
			free( ll2 );
		}
	}
	thisList->N          = 0;
	thisList->first      = NULL;
	thisList->last       = NULL;
	thisList->nextInLoop = NULL;
}

void List2Array( list *thisList, void **array ) {

	listElement *ll;
	int                      i = 0;

	if( ! thisList ) return;
	if( thisList->N == 0 ) return;

	ll = thisList->first;
	while( ll ) {
		array[i] = ll->pointer;
		i++;
		ll = ll->next;
	}
}

void  AddFirst( list *thisList, void *pointer) {

	if( ! thisList ) {
		fprintf(stderr, "[W] list: Adding element to NULL pointer\n");
		return;
	}

	if( !thisList->N ) {
		thisList->first         = CreateListElement( pointer);
		thisList->last          = thisList->first;
	}
	else {
		thisList->first         = AddBeforeElement( thisList->first, pointer);
	}
	thisList->N++;
}

void  AddLast ( list *thisList, void *pointer) {

	if( ! thisList ) {
		fprintf(stderr, "[W] list: Adding element to NULL pointer\n");
		return;
	}

	if( !thisList->N ) {
		thisList->first         = CreateListElement( pointer);
		thisList->last          = thisList->first;
	}
	else {
		thisList->last          = AddAfterElement( thisList->last, pointer);
	}
	thisList->N++;
}

void *StartLoop( list *thisList ) {

	if( ! thisList ) return NULL;

	if( !thisList->N ) return NULL;

	thisList->nextInLoop = thisList->first->next;
	return thisList->first->pointer;
}

void *GetNext ( list *thisList ) {

	void    *dataPointer;

	if( thisList->nextInLoop == NULL ) {
		dataPointer = NULL;
	}
	else {
		dataPointer = thisList->nextInLoop->pointer;
		thisList->nextInLoop = thisList->nextInLoop->next;
	}
	return dataPointer;
}

int   IsInList( list *thisList, void *dataPointer ) {

	listElement *ll;

	if( ! thisList ) return FALSE;

	if( thisList->N ) {
		ll = thisList->first;
		while( ll ) {
			if( dataPointer == ll->pointer) return TRUE;
			ll = ll->next;
		}
	}
	return FALSE;
}

int   ElementsInList( list *thisList ) {

	if( ! thisList ) return 0;
	return thisList->N;
}

void  RemoveFromList( list *thisList, void *dataPointer ) {

	listElement *ll;

	if( ! thisList ) return;

	if( thisList->N ) {
		ll = thisList->first;
		while( ll ) {
			if( dataPointer == ll->pointer ) {
				if( thisList->last == thisList->first ) {
					thisList->N          = 0;
					ll->pointer          = NULL;
				}
				else {
					if( ll == thisList->first )
						thisList->first = ll->next;
					else if( ll == thisList->last )
						thisList->last = ll->previous;
					DestroyListElement( ll );
					thisList->N--;
				}
				return;
			}
			ll = ll->next;
		}
	}
}

void PrintList( list *thisList ) {

	listElement *ll;

	if( ! thisList ) {
		fprintf(stderr, "[W] list: NULL list \n");
		return;
	}
	printf("%lu elements:\n", thisList->N);
	if( thisList->N ) {
		ll = thisList->first;
		while( ll ) {
			printf(" %p %d %p \n",ll->previous,*(int *)ll->pointer,ll->next );
			ll = ll->next;
		}
	}
	printf("===============\n");
}
