/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
	int job_id;
	int arrival_time;
	int running_time;
	int priority;
	int remaining_time;
	int first_time_on_core;
	int last_recorded_time;

} job_t;

scheme_t scheme_used;
priqueue_t * waiting_list;
job_t** job_in_cores;
int cores_number;

int total_num = 0;
int total_response_time = 0;
int total_turnaround_time = 0;
int total_waiting_time = 0;

int comparer(const void * ptr1, const void * ptr2)
{
	int result = 0;
	int timediff = ((job_t*)ptr1)->arrival_time - ((job_t*)ptr2)->arrival_time;
	if(scheme_used == FCFS)
	{
	    return timediff;
	}
	else if(scheme_used == SJF)
	{
	    result = ((job_t*)ptr1)->running_time - ((job_t*)ptr2)->running_time;
	    return (result==0) ? timediff : result;
	}
	else if(scheme_used == PSJF)
	{
    	result = ((job_t*)ptr1)->remaining_time - ((job_t*)ptr2)->remaining_time;
    	return (result==0) ? timediff : result;
	}
	else if(scheme_used == PRI || scheme_used == PPRI)
	{
    	result = ((job_t*)ptr1)->priority - ((job_t*)ptr2)->priority;
    	return (result==0) ? timediff : result;
	}
	else if(scheme_used == RR)
	{
    	return 0;
	}
	return 0;
}

void update_time(int time)
{
	job_t * temp = NULL;
	int i;
	for(i=0; i < cores_number; i++)
	{
		temp = job_in_cores[i];
		if(temp != NULL){
			if((temp->first_time_on_core == -1) && (temp->last_recorded_time != time))
			{
				temp->first_time_on_core = temp->last_recorded_time;
	        	total_response_time += (temp->first_time_on_core - temp->arrival_time);
			}

		temp-> remaining_time -= time - temp->last_recorded_time;
		temp->last_recorded_time = time;

		}
	}  
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/

void scheduler_start_up(int cores, scheme_t scheme)
{
	scheme_used = scheme;
	waiting_list = (priqueue_t*)malloc(sizeof(priqueue_t));
	priqueue_init(waiting_list,&comparer);
	cores_number = cores;
	job_in_cores = (job_t**)malloc(sizeof(job_t*)*cores);
	int i;
	for(i=0;i<cores;i++) job_in_cores[i] = NULL;
}

int new_job_arrival_helper(job_t * newjob)
{
	int i;
	if(scheme_used == PSJF)
	{
		int time = newjob->remaining_time;
		int longest_i = -1;
		int longest_time = -1;
		int arrival_time_now = -1;
		for(i=0;i<cores_number;i++)
		{
			if((job_in_cores[i]->remaining_time > time) && (job_in_cores[i]->remaining_time > longest_time))
			{
				longest_i = i;
				longest_time = job_in_cores[i]->remaining_time;
				arrival_time_now = job_in_cores[i]->arrival_time;
			}
			else if((job_in_cores[i]->remaining_time > time) && ((job_in_cores[i]->remaining_time == longest_time)))
			{
				if(job_in_cores[i]->arrival_time > arrival_time_now)
				{
					longest_i = i;
					longest_time = job_in_cores[i]->remaining_time;
					arrival_time_now = job_in_cores[i]->arrival_time;
				}
			}
		}
		return longest_i;
	}
	else
	{
		int pri = newjob->priority;
		int largest_i = -1;
		int largest_pri = -1;
				int arrival_time_now = -1;
		for(i=0;i<cores_number;i++)
		{
			if((job_in_cores[i]->priority > pri) && (job_in_cores[i]->priority > largest_pri))
			{
				largest_i = i;
				largest_pri = job_in_cores[i]->priority;
				arrival_time_now = job_in_cores[i]->arrival_time;
			}
			else if((job_in_cores[i]->priority > pri) && (job_in_cores[i]->priority == largest_pri))
			{
				if(job_in_cores[i]->arrival_time > arrival_time_now)
				{
					largest_i = i;
					largest_pri = job_in_cores[i]->priority;
					arrival_time_now = job_in_cores[i]->arrival_time;
				}
			}
		}
		return largest_i;
	}
}

/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	update_time(time);
	total_num++;
	job_t * newjob = malloc(sizeof(job_t));
	newjob->job_id = job_number;
	newjob->arrival_time = time;
	newjob->running_time = running_time;
	newjob->remaining_time = running_time;
	newjob->priority = priority;
	newjob->first_time_on_core = -1;
	newjob->last_recorded_time = -1;

	int i;
	for(i=0;i<cores_number;i++)
	{
		if(job_in_cores[i] == NULL)
		{
			job_in_cores[i] = newjob;
			newjob->last_recorded_time = time;
			return i;
		}
	}
	
	if((scheme_used != PSJF) && (scheme_used != PPRI))
	{
		priqueue_offer(waiting_list, newjob);
		return -1;
	}
	else
	{
		int temp = new_job_arrival_helper(newjob);
		if(temp == -1)
		{
			priqueue_offer(waiting_list, newjob);
			return -1;
		} 
		else
		{
			priqueue_offer(waiting_list, job_in_cores[temp]);
			job_in_cores[temp] = newjob;
			newjob->last_recorded_time = time;
			return temp;
		}
	}
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	update_time(time);
	total_turnaround_time += time - job_in_cores[core_id]->arrival_time;
	total_waiting_time += time - job_in_cores[core_id]->arrival_time - job_in_cores[core_id]->running_time;
	free(job_in_cores[core_id]);
	job_in_cores[core_id] = NULL;
	if(waiting_list->head == NULL) return -1;
	else
	{
		job_t * job_todo = priqueue_poll(waiting_list);
		job_in_cores[core_id] = job_todo;
		job_todo->last_recorded_time = time;
		return job_todo->job_id;
	}
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	update_time(time);
	if(waiting_list->head == NULL)
	{
		return job_in_cores[core_id]->job_id;
	}
	else
	{
		job_t * job_todo = priqueue_poll(waiting_list);
		priqueue_offer(waiting_list, job_in_cores[core_id]);
		job_in_cores[core_id] = job_todo;
		job_todo->last_recorded_time = time;
		return job_todo->job_id;
	}
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return (float)total_waiting_time / (float)total_num;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return (float)total_turnaround_time / (float)total_num;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return (float)total_response_time / (float)total_num;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
	free(waiting_list);
	free(job_in_cores);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{

}
