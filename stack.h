/*
 * stack.h
 *
 *  Created on: 3/11/2018
 *      Author: Jamie Getty
 *      Email : jamiegetty1@gmail.com
 */

#ifndef STACK_H_
#define STACK_H_

/*#include <stddef.h>*/
/*#include <stdint.h>*/
#include <string.h>
/*#include <stdlib.h>*/
#include <time.h>

/*
 * ----------------------------------------------------------------------
 * Constants
 * ----------------------------------------------------------------------
 */

/*
 * The maximum size our stack can be. Since this depends on SIZE_MAX, this is platform-dependent
 */
#define _STACK_SIZE_MAX SIZE_MAX


/*
 * ----------------------------------------------------------------------
 * Privately used macros
 * ----------------------------------------------------------------------
 */


/*
 * Resize the stack. This is done by reallocating the memory for the buffer
 * If reallocation fails, then we just don't increase the size and do nothing
 */
#define _STACK_RESIZE(stack, newSizeBytes, type) \
do \
{\
	type *newContents = realloc(stack.contents, newSizeBytes);\
	if (newContents != NULL)\
	{\
		stack.contents = newContents;\
		STACK_CAPACITY(stack) = newSizeBytes / stack.elementSizeBytes;\
	}\
} while (0)


/*
 * Swap two elements in the stack. This is useful for things like shuffling
 * If either index is out of range of the stack, then no swap will occur
 */
#define _STACK_SWAP_ELEMENTS(stack, type, indexA, indexB) \
do \
{\
	if (indexA < stack.size && indexB < stack.size)\
	{\
		type temp;\
		temp = stack.contents[indexA];\
		stack.contents[indexA] = stack.contents[indexB];\
		stack.contents[indexB] = temp;\
	}\
} while (0)

/*
 * ----------------------------------------------------------------------
 * Publicly used macros
 * ----------------------------------------------------------------------
 */

/*
 * Define a stack called 'name' that holds elements of type 'type'.
 *
 * @name: The name of this structure (optional)
 * @type: The type of object being kept in this stack (e.g. int, char, struct doodah, struct doodah* etc.)
 *
 * @size: 	  The size (total number of elements) of the stack
 * @capacity: The capacity (maximum number of elements the stack could currently hold) of the stack
 * @elementSizeBytes: The size of each element of the stack, in Bytes. This is used to keep track during operations
 * @top:  	  Pointer to the top element of the stack
 * @contents: Array of 'type', which holds the data
 */
#define STACK(name, type) \
struct name { \
	size_t size; \
	size_t capacity; \
	size_t elementSizeBytes; \
	type *top; \
	type *contents; \
}


/*
 * Get the size of the stack - This is the number of elements in the stack
 * This is not to be confused with the capacity, which is the maximum number of elements the stack could currently hold
 *
 * @stack: A STACK structure we want the size of
 */
#define STACK_SIZE(stack) \
(stack.size)


/*
 * Get the capacity of the stack - This is the maximum number of elements the stack could currently hold
 * This is not to be confused with the size, which is the number of elements in the stack
 *
 * @stack: A STACK structure we want the capacity of
 */
#define STACK_CAPACITY(stack) \
(stack.capacity)


/*
 * Set a pointer to the top of the stack
 *
 * @stack: A STACK structure we want the capacity of
 */
#define STACK_GET_TOP(stack, p) \
(p = stack.top)


/*
 * Initialize a stack that was created with the STACK macro. Sets default values for all the fields.
 * Also set the seed for RNG
 *
 * @stack: A STACK structure that needs initializing
 *
 */
#define STACK_INIT(stack) \
STACK_SIZE(stack) = 0; \
STACK_CAPACITY(stack) = 1; \
stack.elementSizeBytes = sizeof(*(stack.top)); \
stack.contents = calloc(1, stack.elementSizeBytes);\
stack.top = stack.contents;\
srand(time(NULL))


/*
 * A loop that iterates over each element in the stack. You should NOT remove stack elements as you do this
 *
 * @stack: A STACK structure that we want to iterate over
 * @p: A pointer to the current element of the stack. This changes as we iterate
 *
 */
#define STACK_FOREACH(stack, p) \
for (p = stack.contents; p < (stack.contents + STACK_SIZE(stack)); p++)


/*
 * A test to see if the stack is empty
 *
 * @stack: A STACK structure we want to test for emptiness
 */
#define STACK_IS_EMPTY(stack) \
(STACK_SIZE(stack) == 0)


/*
 * A test to see if the stack is full
 *
 * @stack: A STACK structure we want to test for fullness
 */
#define STACK_IS_FULL(stack) \
(STACK_SIZE(stack) == STACK_CAPACITY(stack))


/*
 * Push an element to the top of the stack.
 * I suppose we could do a size check whenever element is passed, but eh. TODO
 *
 * @stack: A STACK structure we want to push something onto
 * @element: The element we want to push onto stack
 * @type: The type of the element we want to push onto the stack. This MUST be the same as element. This is used for the _STACK_RESIZE macro
 */
#define STACK_PUSH(stack, element, type) \
if (STACK_IS_FULL(stack)) \
{ \
	_STACK_RESIZE(stack, stack.elementSizeBytes * STACK_CAPACITY(stack) * 2, type); \
}\
stack.contents[STACK_SIZE(stack)] = element;\
stack.top = stack.contents;\
stack.top = stack.top + STACK_SIZE(stack);\
(STACK_SIZE(stack))++;\


/*
 * Pop (remove from top) an element from the stack.
 *
 * @stack: The STACK structure we want to pop an element from
 * @element: Will be set to a copy of the popped element.
 * NOTE: This is malloc'd, so you must free it yourself when you are done. No exceptions. Got it?
 *
 * If the stack size is zero, then we do nothing
 * Otherwise, we copy the top element, then delete the original
 * TODO: Should we reduce the size of the buffer once we're down a certain amount? What about when the stack is empty?
 */
#define STACK_POP(stack, element)\
if (!STACK_IS_EMPTY(stack))\
{\
	element = malloc(stack.elementSizeBytes);\
	*element = *(stack.top);\
	memset(stack.top, 0, stack.elementSizeBytes);\
	(STACK_SIZE(stack))--;\
	(stack.top)--;\
}\



/*
 * Randomize the elements of the stack using Fisher-Yates
 * Random number generation is done with the standard library
 *
 * @stack: The stack we want to shuffle
 * @type: The type of element being stored in the stack. Needed for swapping elements
 */
#define STACK_SHUFFLE(stack, type)\
do\
{\
	size_t i;\
	for (i = 0; i < (stack.size - 1); i++)\
	{\
		size_t j = rand() % (stack.size - 1) + i;\
		_STACK_SWAP_ELEMENTS(stack, type, i, j);\
	}\
} while(0)


/*
 * Get an element of the stack at a specified index (but only copy it)
 * TODO: Handle what happens when the index is too large
 *
 * @stack: The stack we are getting an element from
 * @index: The index of the element we want
 * @element: Variable that will store the element
 */
#define STACK_GET_INDEX(stack, index, element)\
if (index < STACK_SIZE(stack))\
{\
	element = (stack.contents)[index];\
}\
else {\
	puts("TODO");\
}


/*
 * Get an element of the stack at a specified index (and remove it)
 * TODO: Handle what happens when the index is too large
 *
 * @stack: The stack we are getting an element from
 * @index: The index of the element we want
 * @element: Variable that will store the element
 */
#define STACK_TAKE_INDEX(stack, index, element) \
do \
{\
	if (index < STACK_SIZE(stack))\
	{\
		element = (stack.contents)[index];\
		\
		int i;\
		for (i = index; i < (STACK_SIZE(stack) - 1); i++) \
		{\
			stack.contents[i] = stack.contents[i+1];\
		}\
		memset(stack.contents + (STACK_SIZE(stack) - 1), 0, stack.elementSizeBytes);\
		(STACK_SIZE(stack))--;\
	}\
	else {\
		puts("TODO");\
	}\
} while (0)


/*
 * Check if the element is in the stack
 * TODO: Implement
 */
#define STACK_IS_IN(stack, element)



/*
 * Insert an element at a specified index in the stack (0 being the base of the stack)
 */


/*
 * Delete everything from the stack, making it empty
 *
 * @stack: The stack we are clearing
 * TODO: Usual questions about what to do with free space
 */
#define STACK_CLEAR(stack)\
memset(stack.contents, 0, STACK_SIZE(stack) * stack.elementSizeBytes);\
STACK_SIZE(stack) = 0;\
stack.top = stack.contents


/*
 * Free all used memory
 *
 * @stack: The stack we are freeing
 */
#define STACK_DELETE(stack)\
(free(stack.contents))

/*
* Same operations for multiple items
*
*Stacks can be for example, one big array, a linked list of nodes with a reference to the top, a linked list of nodes each of which has a fixed size array, etc.
*	Basically, a stack can be made of a ton of other types of data structure.
*	Arrays are good for cache, but also limit the size of your list. linked list based stacks can be arbitrarily big, but memory contiguity could be a problem for fat stacks
*	Looking at the Doom 3 source code for Stack.h indicates that Id Software likes the node-based approach, rather than the array-based approach
*	Arduino's Queue library seems to use an array-based approach with void pointers, rather than defining the data type with macros like OpenBSD does
*	There is no perfect solution. It depends on your application
*		Doom 3's node-based approach is fine because you're writing desktop software with lots of memory
*			<vector> in C++ is also dynamically allocated. When the internal buffer runs out, a new one that's twice the size is created.
*		Arduino's array-based approach works better for smaller platforms
*
*		Should we store the reference to the data or the entirety of the data. It makes sense to copy data. This means that we should be doing memcpy stuff
* */

#endif /* STACK_H_ */
