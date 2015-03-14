/** @file shell.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "log.h"


log_t Log;
pid_t pid;
char * cwd;
char * buffer;
char * command;
size_t commandsize;




/**
 * Starting point for shell.
 */


char* getline_(char * input, size_t size){

	if(input != NULL)
	{
		free(input);
		input = NULL;
	}

	pid = getpid();
	free(buffer);
	buffer = malloc(1024 * sizeof(char));
	cwd = getcwd(buffer,1024);
	printf("(pid=%d)%s$ ", pid, cwd);

	free(input);
	//free(size);
	getline(&input, &size, stdin);
	input[strlen(input)-1] = '\0';

	return input;
}

int main() {
	log_init(&Log);

	//free(buffer);

	//printf("(pid=%d)%s$ ", pid, cwd);

	
	command = getline_(command,commandsize);
	
	//if(command == NULL) printf("NULL\n");
	//printf("%s\n",command);
	//printf("%zd\n",commandsize);

	while(strcmp(command,"exit") != 0)
	{

		//check if command is empty
		if(command[0] == '\0')
		{
			command = getline_(command,commandsize);
			continue;
		}
		//end

		//check if command is cd xxx
		else if(strncmp(command,"cd ",3) == 0)
		{
			log_push(&Log, command);
			printf("Command executed by pid=%d\n", pid);
			int error = chdir(command+3);
			
			if(error != 0)
			{
				printf("%s",command+3);
				printf(": No such file or directory\n");
			}

			command = getline_(command,commandsize);
			continue;
		}
		//end
		
		//check if command is !#	
		else if(strcmp(command,"!#") == 0)
		{
			printf("Command executed by pid=%d\n", pid);
			print_log(&Log);

			command = getline_(command,commandsize);
			continue;
		}
		//end

		//check if command is !query
		else if(strncmp(command,"!",1) == 0)
		{
			printf("Command executed by pid=%d\n", pid);
			
			char* searchresult = log_search(&Log, command+1);
			if(searchresult == NULL)
			{
				printf("No Match\n");
				command = getline_(command,commandsize);
				continue;
			}
			else
			{
				printf("%s matches %s\n",command+1, searchresult);
				strcpy(command,searchresult);
				continue;
			}
		}
		//end

		//non bulid in
		else
		{
			log_push(&Log, command);
			int status = -1;	
			//string process			
			char * str[100];
 			char * pch;
			pch = strtok (command," ");
			int i = 0;
			while (pch != NULL)
			{
				str[i] = pch;
				pch = strtok (NULL, " ");
				i++;
			}
			str[i] = NULL;
			//end

			pid_t newpid = fork();
			if(newpid == 0)
			{
				printf("Command executed by pid=%d\n", getpid());
				int error2 = execvp(str[0], str);
				if(error2 == -1)
				{
					printf("%s: not found\n",str[0]);
					return 0;
				}
			}
			else
			{
				waitpid(newpid,&status, 0);
			}
			command = getline_(command,commandsize);
			continue;
		}
		

	}
	if(buffer != NULL) free(buffer);
	//if(cwd != NULL) free(cwd);
	if(command != NULL)free(command);
	log_destroy(&Log);

    return 0;
}
