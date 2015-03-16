#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/**
 * @file   list.h
 * @author Students of "visualization"
 * @date   Tue Nov 22 10:41:32 2005
 *
 * @brief  Functions dealing with lists.


 Lists store pointers to any kind of object (polygon*, line *, int *, etc)

 Elements of one list are all of the same type. The application is
 responsible of knowing which type is a particular list of.

 Examples of use.

 - To create a list

 <code>
 list *l; \n
 l = createVoidList(); // Create the structure \n // The list is empty, but ready for be used

 </code>

 - To add elements to a list (of trfm3D transformations)

 We can use two functions: AddFirst(list *, void *) and AddLast(list *, void *).
 In both cases, list * must be a valid (created) list and void * a pointer
 to any structure.

 <code>
 // Fill the list with 10 transformations

 trfm3D * transf; \n // Pointer to a trfm3D transformation
 for (int i=0; i<10; i++) {\n
 transf = CreateTrfm3D(); // Create the transformation \n
 ... // make something with tranfs \n
 AddLast(l, transf); // Adds at the end of the list \n
 } \n
 </code>

 - To iterate over a list (of trfm3D transformations):

 <code>
 list * l; \n
 trfm3D * oneElement; \n
 float P[3] = {1.0, 1.0, 1.0}; // Point at (1,1,1) \n
 oneElement = StartLoop( l ); // Start iterating the list \n
 while( oneElement ) { \n
 TransformPoint(oneElement, &P[0], &P[1], &P[2]); // Apply transformation to point \n
 oneElement = GetNext( l ); // Next element of the list. NULL if last element.\n
 }
 </code>

 In this example

 oneElement: auxiliar pointer pointing to the elements of the list

 l: list

 void *StartLooop(list *) : returns a pointer to the first element of the list. NULL if list is empty.

 void *GetNext(list *) : returns a pointer to the next element of the

 list. NULL if accessing past-to-the-end of list.

*/

typedef struct  {
	void    *previous; /*!< Null means first element */
	void    *next;     /*!< Null means last element */
	void    *pointer;
} listElement;

void         DestroyListElement( listElement * );
listElement *CreateListElement( void * );

listElement *AddBeforeElement( listElement *, void * );
listElement *AddAfterElement( listElement *, void * );

typedef struct {
	size_t       N;  /*!< Num. of elements in list */
	listElement  *first;
	listElement  *last;
	listElement  *nextInLoop;
} list;


/*! Before destroying the list,
  probably, you should destroy the elements the list points to.
  If they are not destroyed, they might be left "dangling"
  if nobody else is pointing to them */

void  DestroyList( list ** );


list *CreateVoidList();

/*! Before Emptying one list
  1) somewhere must be kept the items the list contains, or
  2) they have just been destroyed */

void  EmptyList( list * );


/*! Passes the pointers to a given array
  the array must be larger than the number of items stored in the list
  usually after doing this transpor, the list is destroyed or emptied */

void List2Array( list *, void ** );

/*! Adds an element in front of the list

  list must point to a valid list (can be empty) */

void  AddFirst( list *, void * );

/*! Adds an element in the end of the list

  list must point to a valid list (can be empty) */

void  AddLast ( list *, void * );

/*! Start a loop for iterating the list
  \return The first element of the list. NULL if list points to a invalid list or list is empty
*/

void *StartLoop( list * );

/*! Get the next element of the list. list must point to a valid list (initialized with StartLoop)
  \return The next element of the list. NULL if accessing past-of-the-end of list.
*/

void *GetNext ( list * );

/*! \return TRUE if element on list. FALSE otherwise
 */

int   IsInList( list * , void * );

/*! \return Number of elements in list
 */

int   ElementsInList( list * );

/*! \return Remove from thisList the element pointed by dataPointer
 */

void  RemoveFromList( list * thisList, void *dataPointer);

void  PrintList( list *thisList );

#endif /* LIST_H */
