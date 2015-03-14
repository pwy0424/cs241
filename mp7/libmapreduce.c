/** @file libmapreduce.c */
/* 
 * CS 241
 * The University of Illinois
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

#include "libmapreduce.h"
#include "libds/libds.h"


static const int BUFFER_SIZE = 2048;  /**< Size of the buffer used by read_from_fd(). */

pthread_t thread_;
struct epoll_event events[10];

int ** pipefds; 
int value_num;

/**
 * Adds the key-value pair to the mapreduce data structure.  This may
 * require a reduce() operation.
 *
 * @param key
 *    The key of the key-value pair.  The key has been malloc()'d by
 *    read_from_fd() and must be free()'d by you at some point.
 * @param value
 *    The value of the key-value pair.  The value has been malloc()'d
 *    by read_from_fd() and must be free()'d by you at some point.
 * @param mr
 *    The pass-through mapreduce data structure (from read_from_fd()).
 */
static void process_key_value(const char *key, const char *value, mapreduce_t *mr)
{
	unsigned long already_exist = datastore_put(mr->data, key, value);
	if (already_exist != 0)
	{
		free((void*)key);
		free((void*)value);	
		return;
	}
	else
	{
		const char * value2 = datastore_get(mr->data, key, &already_exist);
		const char * result = mr->myreduce(value, value2);
		datastore_update(mr->data, key, result, already_exist);
		free((void*)value2);
		free((void*)result);
	}
	free((void*)key);
	free((void*)value);
}


/**
 * Helper function.  Reads up to BUFFER_SIZE from a file descriptor into a
 * buffer and calls process_key_value() when for each and every key-value
 * pair that is read from the file descriptor.
 *
 * Each key-value must be in a "Key: Value" format, identical to MP1, and
 * each pair must be terminated by a newline ('\n').
 *
 * Each unique file descriptor must have a unique buffer and the buffer
 * must be of size (BUFFER_SIZE + 1).  Therefore, if you have two
 * unique file descriptors, you must have two buffers that each have
 * been malloc()'d to size (BUFFER_SIZE + 1).
 *
 * Note that read_from_fd() makes a read() call and will block if the
 * fd does not have data ready to be read.  This function is complete
 * and does not need to be modified as part of this MP.
 *
 * @param fd
 *    File descriptor to read from.
 * @param buffer
 *    A unique buffer associated with the fd.  This buffer may have
 *    a partial key-value pair between calls to read_from_fd() and
 *    must not be modified outside the context of read_from_fd().
 * @param mr
 *    Pass-through mapreduce_t structure (to process_key_value()).
 *
 * @retval 1
 *    Data was available and was read successfully.
 * @retval 0
 *    The file descriptor fd has been closed, no more data to read.
 * @retval -1
 *    The call to read() produced an error.
 */
static int read_from_fd(int fd, char *buffer, mapreduce_t *mr)
{
	/* Find the end of the string. */
	int offset = strlen(buffer);

	/* Read bytes from the underlying stream. */
	int bytes_read = read(fd, buffer + offset, BUFFER_SIZE - offset);
	if (bytes_read == 0)
		return 0;
	else if(bytes_read < 0)
	{
		fprintf(stderr, "error in read.\n");
		return -1;
	}

	buffer[offset + bytes_read] = '\0';

	/* Loop through each "key: value\n" line from the fd. */
	char *line;
	while ((line = strstr(buffer, "\n")) != NULL)
	{
		*line = '\0';

		/* Find the key/value split. */
		char *split = strstr(buffer, ": ");
		if (split == NULL)
			continue;

		/* Allocate and assign memory */
		char *key = malloc((split - buffer + 1) * sizeof(char));
		char *value = malloc((strlen(split) - 2 + 1) * sizeof(char));

		strncpy(key, buffer, split - buffer);
		key[split - buffer] = '\0';

		strcpy(value, split + 2);

		/* Process the key/value. */
		process_key_value(key, value, mr);

		/* Shift the contents of the buffer to remove the space used by the processed line. */
		memmove(buffer, line + 1, BUFFER_SIZE - ((line + 1) - buffer));
		buffer[BUFFER_SIZE - ((line + 1) - buffer)] = '\0';
	}

	return 1;
}


/**
 * Initialize the mapreduce data structure, given a map and a reduce
 * function pointer.
 */
void mapreduce_init(mapreduce_t *mr, 
                    void (*mymap)(int, const char *), 
                    const char *(*myreduce)(const char *, const char *))
{	
	mr->data = malloc(sizeof(datastore_t));
	datastore_init(mr->data);

	mr->mymap = mymap;
	mr->myreduce = myreduce;
}



void * reduce_helper(void * input)
{
	mapreduce_t* mr = (mapreduce_t*) input;
	int epfd = epoll_create(10);

	char ** buffers = malloc(value_num * sizeof(char *));
	int i;
	for(i = 0; i < value_num; i++)
	{
		buffers[i] = malloc(BUFFER_SIZE + 1);
		buffers[i][0] = '\0';
		events[i].events = EPOLLIN;
		events[i].data.fd = pipefds[i][0];
		epoll_ctl(epfd, EPOLL_CTL_ADD, pipefds[i][0], &events[i]);
	}

	int count = 0;

	while(count < value_num)
	{
		struct epoll_event ev;
		epoll_wait(epfd, &ev, 1, -1);
		int temp;
		for (i = 0; i < value_num; i++)
		{
			if (pipefds[i][0] == ev.data.fd) temp = i;
		}

		int check = read_from_fd(ev.data.fd,buffers[temp],mr);
			//strcpy(mr->buffers[i],buffer);

		if(check == 0)
		{
			count++;
			epoll_ctl(epfd,EPOLL_CTL_DEL,ev.data.fd,NULL);
		}
		else if(check == 1);
		else epoll_ctl(epfd,EPOLL_CTL_DEL,ev.data.fd,NULL);


	}
		
	for(i = 0; i < value_num; i++)
	{
		free(buffers[i]);
	}
	free(buffers);
	return NULL;

}


/**
 * Starts the map() processes for each value in the values array.
 * (See the MP description for full details.)
 */
void mapreduce_map_all(mapreduce_t *mr, const char **values)
{
	value_num = 0;
	pid_t newpid;
	while(values[value_num] != NULL) value_num++;
	
	pipefds = malloc(sizeof(int *) * value_num);

	int i;
	for(i = 0; i < value_num; i++)
	{
		pipefds[i] = malloc(sizeof(int) * 2);
		pipe(pipefds[i]);
		newpid=fork();
		if(newpid == 0)
		{
				close(pipefds[i][0]);
				mr->mymap(pipefds[i][1], values[i]);
				exit(0);
		}
		else close(pipefds[i][1]);
	}

	pthread_create(&thread_, NULL, reduce_helper, (void*) mr);

}


/**
 * Blocks until all the reduce() operations have been completed.
 * (See the MP description for full details.)
 */
void mapreduce_reduce_all(mapreduce_t *mr)
{
	mapreduce_t * shit = mr;
	if(shit == mr)pthread_join(thread_, NULL);
}


/**
 * Gets the current value for a key.
 * (See the MP description for full details.)
 */
const char *mapreduce_get_value(mapreduce_t *mr, const char *result_key)
{
	return datastore_get(mr->data, result_key, NULL);
}


/**
 * Destroys the mapreduce data structure.
 */
void mapreduce_destroy(mapreduce_t *mr)
{
	int i;
	for(i = 0; i < value_num; i++)
	{
		free(pipefds[i]);
	}
	free(pipefds);
	datastore_destroy(mr->data);
	free(mr->data);
}
