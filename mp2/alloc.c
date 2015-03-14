/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>



typedef struct _mem_dictionary 
{
	//void *addr;
	size_t size;
	//int free;
	struct _mem_dictionary* next;
	//struct _mem_dictionary* prev;
} mem_dictionary;

//mem_dictionary *head = NULL;
mem_dictionary *hole = NULL;
mem_dictionary *tail = NULL;
//mem_dictionary *tail = NULL;
//int dictionary_ct = 0;


/**
 * Allocate space for array in memory
 * 
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 * 
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
	/* Note: This function is complete. You do not need to modify it. */
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size)
{
	//printf("malloc size:%zd\n",size);
	if(size == 0) return NULL;
	mem_dictionary *temp = hole;
	
	if(hole != NULL)
	{
		if(hole->size >= size) //&& (hole->size-size-sizeof(mem_dictionary) < 1))// && (hole->size <= size + 1000*sizeof(mem_dictionary)))
		{
			hole = hole->next;
			//temp->next = head;
			//head = temp;

			//mem_dictionary *newcell = (void*)hole + size + sizeof(mem_dictionary);
			//newcell->size = hole->size - size - sizeof(mem_dictionary);
			//newcell->next = hole;
			//hole = newcell;

			//temp->size = size;

			return (void*)temp+sizeof(mem_dictionary);
		}
		else
		{
			while(temp->next != NULL)
			{
				if(temp->next->size >= size) //&& (temp->next->size-size-sizeof(mem_dictionary) < 1))// && (temp->next->size <= size + 1000*sizeof(mem_dictionary)))
				{
					mem_dictionary *touse = temp->next;
					temp->next = touse->next;
					//touse->size = size;
					//touse->next = head;
					//head = touse;
					//mem_dictionary *newcell = (void*)touse + size + sizeof(mem_dictionary);
					//newcell->size = touse->size - size - sizeof(mem_dictionary);
					//newcell->next = hole;
					//hole = newcell;
					
					//touse->size = size;
					//printf("%zd\n",touse->size);

					return (void*)touse+sizeof(mem_dictionary);
				}
				temp = temp->next;
			}
		}
	}
	

	void *p = sbrk(0);
	p = sbrk(size+sizeof(mem_dictionary));
	
	if(p == (void*)-1) return NULL;
	
	mem_dictionary *tail = p;
	mem_dictionary *cell = p;

	//cell->addr = p+sizeof(mem_dictionary);
	cell->size = size;
	//cell->free = 0;
	//cell->next = NULL;
	cell->next = NULL;
	//head = cell;

	return (void*)cell+sizeof(mem_dictionary);

}


/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr)
{
	// "If a null pointer is passed as argument, no action occurs."
	if (!ptr)
		return;
	
	//if(head == NULL) return;	

	/*if(head->addr == ptr)
	{
		mem_dictionary *tofree = head;
		head = head->next;
		tofree->next = hole;
		hole = tofree;
		return;
	}

	mem_dictionary *p = head;
	
	while(p->next != NULL)
	{
		if(p->next->addr == ptr)
		{
			mem_dictionary *tofree2 = p->next;
			p->next = tofree2->next;
			tofree2->next = hole;
			hole = tofree2;
			return;
		}	
		p = p->next;
	}	*/

	mem_dictionary *tofree = ptr-sizeof(mem_dictionary);
	//if(hole == NULL)
	//{
		tofree->next = hole;
		hole = tofree;
	//}
	/*else
	{
		mem_dictionary *temp = hole;

		if(temp + temp->size + sizeof(mem_dictionary) == tofree)
			{
				temp->size = temp->size + tofree->size + sizeof(mem_dictionary);
				return;
			}
		if(tofree + tofree->size + sizeof(mem_dictionary) == temp)
		{
			tofree->size = tofree->size + temp->size + sizeof(mem_dictionary);
			tofree->next = temp->next;
			temp->next = NULL;
			hole = tofree;
			return;
		}

		while(temp->next != NULL)
		{
			if(temp->next + temp->next->size + sizeof(mem_dictionary) == tofree)
			{
				temp->next->size = temp->next->size + tofree->size + sizeof(mem_dictionary);
				return;
			}
			if(tofree + tofree->size + sizeof(mem_dictionary) == temp->next)
			{
				tofree->size = tofree->size + temp->size + sizeof(mem_dictionary);
				temp->next = temp->next->next;
				tofree->next = hole;
				hole = tofree;
				return;
			}
			temp = temp->next;
		}
	}*/
	return;
}


/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
	//printf("realloc%zd\n",size);
	 // "In case that ptr is NULL, the function behaves exactly as malloc()"
	if (!ptr)
		return malloc(size);

	 // "In case that the size is 0, the memory previously allocated in ptr
	 //  is deallocated as if a call to free() was made, and a NULL pointer
	 //  is returned."
	if (!size)
	{
		free(ptr);
		return NULL;
	}

	//mem_dictionary *p = head;
	
	
	//while(p)
	//{
	mem_dictionary *p = ptr-sizeof(mem_dictionary);
	//printf("oldsize:%zd\n",p->size);
	//printf("newsize:%zd\n",size);
	if(p->size == size) return ptr;
		
	if(p->size > size)
	{
		/*if(p->size - size - sizeof(mem_dictionary) < 1) return ptr;
		else
		{
			mem_dictionary *newcell = ptr+size;
			newcell->size = p->size - size - sizeof(mem_dictionary);
			newcell->next = hole;
			hole = newcell;
			p->size = size;
			return ptr;
		}*/
		return ptr;
	}

	if(p->size < size)
	{
		free(ptr); 

		void* newp = malloc(size);
		memcpy(newp, ptr, p->size);
		
		return newp;
	}
	//free(ptr);
	//return malloc(size);
		
		//}

		//p = p->next;
	return ptr;	

}
