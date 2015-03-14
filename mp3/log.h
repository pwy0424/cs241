/** @file log.h */

#ifndef __LOG_H_
#define __LOG_H_

typedef struct _log_t {
	struct _log_node * head;
	struct _log_node * tail;
} log_t;

typedef struct _log_node {
	char * value;
	struct _log_node * next;
	struct _log_node * prev;


} log_node;

void log_init(log_t *l);
void log_destroy(log_t* l);
void log_push(log_t* l, const char *item);
char *log_search(log_t* l, const char *prefix);
void print_log(log_t* l);
void log_destroy(log_t* l);

#endif
