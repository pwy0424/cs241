/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->head = NULL;
	q->tail = NULL;
	q->comparer = comparer;
	q->size = 0;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	q->size++;
	priqueue_node * newnode = malloc(sizeof(priqueue_node));
	newnode->data = ptr;
	if(q->head == NULL)
	{
		newnode->next = NULL;
		newnode->prev = NULL;
		q->head = newnode;
		q->tail = newnode;
		return 0;
	}
	else
	{
		if(q->comparer(ptr,q->head->data) < 0)
		{
			q->head->prev = newnode;
			newnode->next = q->head;
			newnode->prev = NULL;
			q->head = newnode;
			return 0;
		}
		else
		{
			int counter = 1;
			priqueue_node * temp = q->head->next;
			while(temp != NULL)
			{
				if(q->comparer(ptr,temp->data) < 0)
				{
					newnode->next = temp;
					newnode->prev = temp->prev;
					temp->prev->next = newnode;
					temp->prev = newnode;
					return counter;
				}
				temp = temp->next;
				counter++;
			}
			q->tail->next = newnode;
			newnode->prev = q->tail;
			newnode->next = NULL;
			q->tail = newnode;
			return counter;
		}
	}
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->head == NULL) return NULL;
	return q->head->data;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->head == NULL) return NULL;
	q->size--;
	void * result = q->head->data;
	priqueue_node * tofree = q->head;
	q->head = q->head->next;
	free(tofree);
	return result;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	priqueue_node * temp = q->head;
	while(temp != NULL)
	{
		priqueue_node * tofree = temp;
		temp = temp->next;
		free(tofree);
	}
}
