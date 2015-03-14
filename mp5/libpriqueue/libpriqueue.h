/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_node
{
	void * data;
	struct _priqueue_node * next;
	struct _priqueue_node * prev;
} priqueue_node;

typedef struct _priqueue_t
{
	priqueue_node * head;
	priqueue_node * tail;
	int(*comparer)(const void *, const void *);
	int size;
} priqueue_t;

void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
