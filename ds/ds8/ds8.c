#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>


/**
 * @define BUFFER_SIZE
 * @brief Size in bytes of the shared buffer
 *
 * This holds the size of the circular buffer shared between producer
 * and consumer
 */
#define BUFFER_SIZE 4

/**
 * @brief Thread specific informations for consumer and producer threads
 *
 * This structure is used to instruct the thread about where is the
 * shared buffer and the semaphore to use.
 */
typedef struct {
	/**
	 * @var buf
	 * @brief The shared buffer
	 *
	 * Producers will fill locations in this buffer, while
	 * consumers will read and print the bytes in the buffer.
	 */
	char * buf;

	/**
	 * @var buf_size
	 * @brief holds the size of the shared ring buffer
	 *
	 * This variable stores the size of the shared ring buffer
	 */
	int buf_size;

	/**
	 * @var open_sem
	 * @brief This semaphore holds the number of free locations
	 *
	 * This semaphore is used by the producer to produce a new
	 * byte in the shared buffer. It will be used by the consumer
	 * to notify that a byte has been consumed from the buffer.
	 */
	sem_t * open_sem;

	/**
	 * @var ready_sem
	 * @brief This semaphore holds the number of ready locations
	 *
	 * This semaphore is used by the producer to notify that a new
	 * byte in the shared buffer has been produced. It will be
	 * used by the consumer to understand that a byte can be
	 * consumed from the buffer.
	 */
	sem_t * ready_sem;
	
} arg_t;

/**
 * @brief Producer routine
 *
 * This is the routine for the producer. It produces one byte at a
 * time and puts it in the shared buffer.
 */
void * producer_thread(void * arg)
{
	/* This thread will perform a loop producing on the buffer
	 * if an available position   */
	int pos = 0;
	
	/* Initialize some data to put on the buffer here */

	arg_t * args = (arg_t *)arg;

	/* Producer loop */
	while(1){
		/* Something goes here ... */
	}
	
	return NULL;
}

/**
 * @brief Consumerer routine
 *
 * This is the routine for the consumer. It consumes one byte at a
 * time from the shared buffer.
 */
void * consumer_thread(void * arg)
{
	/* This thread will perform a loop consuming from the buffer
	 * if a byte is available */
	int pos = 0;
	char read;
	arg_t * args = (arg_t *)arg;

	/* Consumer loop */
	while(1){
		/* Something goes here as well...  */
	}
	
	return NULL;
}

/**
 * @brief Main routine
 *
 * The main will have to:
 * 1. Parse the parameters
 * 2. Initialize the semaphore
 * 3. Initialize the shared buffer
 * 4. Span producers and consumers
 * 
 * @param argc number of parameters
 * @param args command line parameters
 * @returns 0 on sucess, 1 on error
 */
int main( int argc, char ** args )
{
	
	/* Variable definition */
	char * buf = (char *)malloc(BUFFER_SIZE);
	arg_t * prod_args = (arg_t *)malloc(sizeof(arg_t));
	arg_t * cons_args = (arg_t *)malloc(sizeof(arg_t));
	sem_t open_sem;
	sem_t ready_sem;
	pthread_t producer, consumer;
	void * thexit;

	/* Initialize semaphores */
	/* Yup, some code here */
	
	/* Initialize thread arguments */
	/* Guess what? Some code!  */
	
	/* Span threads */
	/* pthread_create stuff */

	/* Wait for thread termination */
	/* Be nice to the thread. Wait for them.  */

	/* Return success */
	return 0;
	
}
