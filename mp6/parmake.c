/** @file parmake.c */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "rule.h"

/**
 * Entry point to parmake.
 */

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;

queue_t * new_rules;

queue_t * all_targets;

queue_t * ready_rules;

void paser_targets(char* targets)
{
	rule_t* newrule = (rule_t*)malloc(sizeof(rule_t));
	rule_init(newrule);

	newrule->target = (char*)malloc(sizeof(char)*200);
	strcpy(newrule->target,targets);

	queue_enqueue(new_rules,newrule);

	queue_enqueue(all_targets,newrule->target);
}

void paser_dependency(char *target, char *dependency )
{
	int i;
	rule_t * ptr = NULL;
	char * depbuf = (char*)malloc(sizeof(char)*200);
	strcpy(depbuf,dependency);
	for(i=0;i<queue_size(new_rules);i++)
	{
		ptr = (rule_t*)queue_at(new_rules,i);
		if(strcmp(ptr->target,target) == 0)
		{
			queue_enqueue(ptr->deps,depbuf);
			return;
		}
	}
	exit(-1);
	return;
}

void paser_command(char *target, char *command)
{
	char* command_buf = (char*)malloc(200);
	strcpy(command_buf,command);
	rule_t * ptr = NULL;
	int i;
	for(i=0;i<queue_size(new_rules);i++)
	{
		ptr = (rule_t*)queue_at(new_rules,i);
		if(strcmp(ptr->target,target) == 0)
		{
			queue_enqueue(ptr->commands,command_buf);
			return;
		}
	}
	exit(-1);
	return;
}

int is_rule(char* item)
{
	int i;
	for(i = 0; i < queue_size(all_targets); i++)
	{
		if(strcmp(item, queue_at(all_targets,i))==0 ) return 1;
	}
	return 0;
}

int rule_ready(char* item)
{
	int i;
	int result = 0;
	pthread_mutex_lock(&mutex2);
	for (i = 0; i < queue_size(ready_rules); i++)
	{
		if( strcmp(item,((rule_t*)queue_at(ready_rules,i)) -> target) == 0 )
		{
			result = 1;
			break;
		}
	}
	pthread_mutex_unlock(&mutex2);
	return result;

}

int deps_ready(queue_t *deps)
{
	if(queue_size(deps) == 0) return 1;

	int i;
	char* item = NULL;
	int size = queue_size(deps);
	for(i = 0; i < size; i++)
	{
		item = queue_at(deps,i);
		if(is_rule(item))
		{
			if(rule_ready(item) == 0) return 0;
		}
		else{
			if(access(item,R_OK) == -1)
			{
				//sprintf(errorbuf,"File %s does not exist.",item);
				exit(-1);
			}
		}
	}
        return 1;

}

rule_t* prepare_rules()
{
	int i;
	int size = queue_size(new_rules);
	rule_t * rule_ptr = NULL;
	int selected_pos = -1;
	if(size == 0) return NULL;
	for(i = 0; i < size; i++){
		rule_ptr = queue_at(new_rules,i);
		if(queue_size(rule_ptr->deps) == 0){
 			selected_pos = i;
			break; 
		}
		else if(deps_ready(rule_ptr->deps)){
			selected_pos = i;
			break;
		}
	}
	if(selected_pos == -1) {
		return NULL;
	}
	else return queue_remove_at(new_rules,selected_pos);
}

void excute_rules(rule_t* rule_ptr)
{
	int i,ret;
	char* command;
	queue_t * commands = rule_ptr -> commands;
	for(i = 0; i < queue_size(commands); i++)
	{
		command = queue_at(commands,i);
		ret = system(command);
		if(ret != 0) exit(-1);
	}
}

int all_deps_are_files(rule_t* rule_ptr)
{
	if(queue_size(rule_ptr->deps) == 0) return 0;

	int i;
	for(i = 0; i < queue_size(rule_ptr->deps); i++)
	{
		if(is_rule(queue_at(rule_ptr->deps,i))) return 0;
	}

	return 1;
}

void* excute(void* ptr)
{
	rule_t* rule_ptr = NULL;
	int remain_size = 0;
	int i;

	do{
		pthread_mutex_lock(&mutex1);
		remain_size = queue_size(new_rules);
		while((rule_ptr = prepare_rules()) == NULL)
		{
			remain_size = queue_size(new_rules);
			if( remain_size > 0) pthread_cond_wait(&cond,&mutex1);
			else break;
		}
		pthread_mutex_unlock(&mutex1);
		if(remain_size == 0) break;

		if(all_deps_are_files(rule_ptr))
		{
			if(access(rule_ptr->target,F_OK) == -1 ) excute_rules(rule_ptr);
			else
			{
				for(i=0;i<queue_size(rule_ptr->deps);i++)
				{
					struct stat s1, s2;
					if (stat(queue_at(rule_ptr->deps,i), &s1) == -1) exit(-1);
					if (stat(rule_ptr->target, &s2) == -1) exit(-1);

					int diff = difftime(s1.st_mtime, s2.st_mtime);
					if( diff >= 0){
						excute_rules(rule_ptr);
						break;
					}
				}
			}
		}
		else excute_rules(rule_ptr);

		pthread_mutex_lock(&mutex2);
		queue_enqueue(ready_rules,rule_ptr);
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mutex2);

                        
	}while(remain_size > 0);

	return NULL;

}

void free_rule(rule_t* p_rule)
{
	char* ptr=NULL;
	while( (ptr = (char*)queue_dequeue(p_rule->deps)) != NULL)
	{
		free(ptr);
	}
	while( (ptr = (char*)queue_dequeue(p_rule->commands)) != NULL)
	{
		free(ptr);
	}

	if(p_rule->target != NULL) free(p_rule->target);

	rule_destroy(p_rule);
	free(p_rule);
}

void free_rule_queue(queue_t * p_queue)
{
	rule_t * ptr = NULL;
	while( (ptr = (rule_t*)queue_dequeue(p_queue)) != NULL )
	{
		free_rule(ptr);
	}
	queue_destroy(p_queue);
	free(p_queue);
}

int main(int argc, char **argv)
{
	int opt,i,next;
	int thread_num = 1;
	char* makefile = NULL;
	char filebuf[100];
	int targets_num = 0;
	char** targets = NULL;

	while ((opt = getopt(argc, argv, "j:f:")) != -1)
	{
		switch (opt) {
			case 'j':
				thread_num = atoi(optarg);
				break;
			case 'f':
				makefile = optarg;
				break;
		}
	}

	if(optind < argc)
	{
		targets_num = argc - optind;
		next = optind;
		targets = (char**)malloc(sizeof(char*)*targets_num+sizeof(char*));
		for(i = 0;i < targets_num; i++)
		{
			targets[i] = malloc(50);
			strcpy(targets[i],argv[next]);
			next ++;
		}
		targets[targets_num] = NULL;
	}

	if(makefile != NULL)
	{
		if(access(makefile,R_OK) == -1){
			return -1;
		}
	}
	else
	{
		if(access("makefile",R_OK) == -1 && access("Makefile",R_OK) == -1) return -1;
		else if(access("makefile",R_OK) == 0)
		{
			strcpy(filebuf,"makefile");
			makefile = filebuf;
		}
		else if(access("Makefile",R_OK) == 0)
		{
			strcpy(filebuf,"Makefile");
			makefile = filebuf;
		}
	}
	
	new_rules = (queue_t*)malloc(sizeof(queue_t));
	all_targets = (queue_t*)malloc(sizeof(queue_t));
	ready_rules = (queue_t*)malloc(sizeof(queue_t));

	queue_init(new_rules);
	queue_init(all_targets);
	queue_init(ready_rules);

	pthread_t* thread_list = (pthread_t*)malloc(sizeof(pthread_t)* thread_num);

	parser_parse_makefile(makefile,targets,&paser_targets,&paser_dependency,&paser_command);

	for(i = 0; i < thread_num; i++)
	{
		pthread_create(&thread_list[i],NULL,excute,NULL);
	}
    
	for(i = 0; i < thread_num; i++)
	{
		pthread_join(thread_list[i],NULL);
	}

	if(targets_num != 0)
	{
		for(i=0;targets[i]!=NULL;i++) free(targets[i]);
		free(targets);
	}

	free_rule_queue(new_rules);
  	free_rule_queue(ready_rules);
    
	queue_destroy(all_targets);
	free(all_targets);
    
	free(thread_list);

	return 0; 
}
