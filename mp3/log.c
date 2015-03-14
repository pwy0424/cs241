/** @file log.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"

/**
 * Initializes the log.
 *
 * You may assuem that:
 * - This function will only be called once per instance of log_t.
 * - This function will be the first function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @returns
 *   The initialized log structure.
 */
void log_init(log_t *l) {
	l->head = NULL;
	l->tail = NULL;
}

/**
 * Frees all internal memory associated with the log.
 *
 * You may assume that:
 * - This function will be called once per instance of log_t.
 * - This funciton will be the last function called per instance of log_t.
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure to be destoryed.
 */
void log_destroy(log_t* l) {
	if(l->head == NULL) return;
	log_node * remove = l->head;
	log_node * temp = l->head->next;
	while(temp != NULL)
	{
		free(remove->value);
		free(remove);
		remove = temp;
		temp = temp->next;
	}
	free(remove->value);
	free(remove);
}

/**
 * Push an item to the log stack.
 *
 *
 * You may assume that:
* - All pointers will be valid, non-NULL pointer.
*
 * @param l
 *    Pointer to the log data structure.
 * @param item
 *    Pointer to a string to be added to the log.
 */
void log_push(log_t* l, const char *item) {
	log_node * new = malloc(sizeof(log_node));

	int length = strlen(item);	

	new->value = malloc((length+1) * sizeof(char));
	strcpy(new->value,item);
	new->next = NULL;
	new->prev = l->tail;
	if(l->head == NULL)
	{
		l->head = new;
		l->tail = new;
	}
	else
	{
		l->tail->next = new;
		l->tail = new;
	}
}


/**
 * Preforms a newest-to-oldest search of log entries for an entry matching a
 * given prefix.
 *
 * This search starts with the most recent entry in the log and
 * compares each entry to determine if the query is a prefix of the log entry.
 * Upon reaching a match, a pointer to that element is returned.  If no match
 * is found, a NULL pointer is returned.
 *
 *
 * You may assume that:
 * - All pointers will be valid, non-NULL pointer.
 *
 * @param l
 *    Pointer to the log data structure.
 * @param prefix
 *    The prefix to test each entry in the log for a match.
 *
 * @returns
 *    The newest entry in the log whose string matches the specified prefix.
 *    If no strings has the specified prefix, NULL is returned.
 */
char *log_search(log_t* l, const char *prefix) {
	log_node * temp = l->tail;
	while(temp != NULL)
	{
		int length = strlen(prefix);
		if(strncmp(temp->value,prefix,length) == 0) return temp->value;
		temp = temp->prev;
	}
    return NULL;
}

void print_log(log_t* l){
	log_node * temp = l->head;
	while(temp != NULL)
	{
		printf("%s\n",temp->value);
		temp = temp->next;
	}
	return;
}

