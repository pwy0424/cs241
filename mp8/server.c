/** @file server.c */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <queue.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "queue.h"
#include "libhttp.h"
#include "libdictionary.h"

const char *HTTP_404_CONTENT = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1>The requested resource could not be found but may be available again in the future.<div style=\"color: #eeeeee; font-size: 8pt;\">Actually, it probably won't ever be available unless this is showing up because of a bug in your program. :(</div></html>";
const char *HTTP_501_CONTENT = "<html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1>The server either does not recognise the request method, or it lacks the ability to fulfill the request.</body></html>";

const char *HTTP_200_STRING = "OK";
const char *HTTP_404_STRING = "Not Found";
const char *HTTP_501_STRING = "Not Implemented";

queue_t new_connection_queue;

/**
 * Processes the request line of the HTTP header.
 * printf("send_file:%s\n", send_file);
 * @param request The request line of the HTTP header.  This should be
 *                the first line of an HTTP request header and must
 *                NOT include the HTTP line terminator ("\r\n").
 *
 * @return The filename of the requested document or NULL if the
 *         request is not supported by the server.  If a filename
 *         is returned, the string must be free'd by a call to free().
 */
char* process_http_header_request(const char *request)
{
    // Ensure our request type is correct...
    if (strncmp(request, "GET ", 4) != 0)
        return NULL;

    // Ensure the function was called properly...
    assert( strstr(request, "\r") == NULL );
    assert( strstr(request, "\n") == NULL );

    // Find the length, minus "GET "(4) and " HTTP/1.1"(9)...
    int len = strlen(request) - 4 - 9;

    // Copy the filename portion to our new string...
    char *filename = malloc(len + 1);
    strncpy(filename, request + 4, len);
    filename[len] = '\0';

    // Prevent a directory attack...
    //  (You don't want someone to go to http://server:1234/../server.c to view your source code.)
    if (strstr(filename, ".."))
    {
        free(filename);
        return NULL;
    }

    return filename;
}



void signal_handler(int signal){
	signal = signal;
	printf("enter handler");
	pthread_t* free_connection = NULL;
	while( (free_connection = (pthread_t*)queue_dequeue(&new_connection_queue)) != NULL)
	{
		//pthread_join(*free_connection,NULL);
		//pthread_detach(*free_connection);
		//pthread_exit(free_connection);
		free(free_connection);
		printf("free_connection");
		//free_connection = NULL;
	}
	
	//if(servinfo != NULL) free(servinfo);   
    //if(i != NULL) free(i); 
	printf("exit handler");
    exit(1);
}




void* workthread(void* input){
    int j;
    int new_fd = *(int*)input;
    FILE* replyfile;
    int die = 0; 

    while(die==0){
        char* next_connection;
        char* filetype;
        char* file;
        char* newfilename;
        int type = 0;
        int requestsize;
        int filesize;
        char buffer[1024];  
        char headline[256];
        requestsize = recv(new_fd, buffer, 1024, 0);
		
		if(requestsize ==0)
		{
			pthread_exit(0);
		}

        for(j = 0; j < requestsize; j++){
            headline[j] = buffer[j];        
            if(buffer[j+1] == '\r' && buffer[j+2] == '\n'){
                headline[j+1]='\0';
                break;
            }
        }
        
        file = process_http_header_request(headline);

        if(file!=NULL){
            if(strcmp(file, "/") == 0){
				free(file);
                file = "/index.html";
            }
        }

        newfilename = malloc(256);
        strcpy(newfilename, "web");
        if(file!=NULL){
            strcat(newfilename, file);
        }
        newfilename[strlen(newfilename)] = '\0';
        die = 0;
        next_connection = "Connection: Keep-Alive";
        if(strstr(buffer, "Connection:") != NULL){
            if (strcasestr(buffer, "connection: close") != NULL){
                die = 1;
                next_connection = "Connection: Close";
            }
        }
        else{
            exit(1);
        }
        if(file == NULL) type = 501;
        else if((replyfile = fopen(newfilename, "r")) == NULL)  type = 404;
        else type = 200;

        if(type == 200){
            struct stat st;
            stat(newfilename, &st);
            filesize = st.st_size;
        	
            if(strstr(file, ".html")) filetype = "text/html";    
            else if(strstr(file, ".css")) filetype = "text/css";
            else if(strstr(file, ".jpg")) filetype = "image/jpeg";    
            else if(strstr(file, ".png")) filetype = "image/png";
            else filetype = "text/plain";
        }
        else
        {
            filetype = "text/html";
        }

        if(type == 200){
            char* http_head_send = malloc(256);
            sprintf(http_head_send, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\ncontent-Length:%d\r\nConnection: %s\r\n\r\n", type, HTTP_200_STRING, filetype, filesize, next_connection);
            char* send_file = malloc(filesize + 1);
            fread(send_file, filesize, 1, replyfile);
			fclose(replyfile);
            send(new_fd, http_head_send, strlen(http_head_send), 0);
            send(new_fd, send_file, filesize, 0);
			//free(replyfile);
			free(send_file);
			free(http_head_send);
        }
        else if(type == 404){
            char* http_head_send = malloc(256);
            filesize = strlen(HTTP_404_CONTENT);
            sprintf(http_head_send, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\ncontent-Length:%d\r\nConnection: %s\r\n\r\n", type, HTTP_404_STRING, filetype, filesize, next_connection);
            send(new_fd, http_head_send, strlen(http_head_send), 0);
            send(new_fd, HTTP_404_CONTENT, strlen(HTTP_404_CONTENT), 0);
			free(http_head_send);
        }
        else if(type == 501){
            char* http_head_send = malloc(256);
            filesize = strlen(HTTP_501_CONTENT);
            sprintf(http_head_send, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\ncontent-Length:%d\r\nConnection: %s\r\n\r\n", type, HTTP_501_STRING, filetype, filesize, next_connection);
            send(new_fd, http_head_send, strlen(http_head_send), 0);
            send(new_fd, HTTP_501_CONTENT, strlen(HTTP_501_CONTENT), 0);
			free(http_head_send);
        }
        else exit(1);


		if(file != NULL && strcmp(file,"/index.html") != 0)
		{
			free(file);
			file = NULL;
		}
		if(newfilename != NULL)
		{
			free(newfilename);
			newfilename = NULL;
		}
    }
	
	return NULL;
}



int main(int argc, char **argv)
{
	queue_init(&new_connection_queue);
	argc = argc;
	argv = argv;
    struct addrinfo temp; 
    struct addrinfo *serverinfo;   
    struct addrinfo *i; 
    struct sockaddr_in client_addr;
    int sockfd, new_fd;   
    int sin_size;   

    memset(&temp, 0, sizeof temp);  
    temp.ai_family = AF_UNSPEC; 
    temp.ai_socktype = SOCK_STREAM; 
    temp.ai_flags = AI_PASSIVE;  

    if ((getaddrinfo(NULL, argv[1], &temp, &serverinfo)) != 0) {
        signal_handler(1);
    }  

    for(i=serverinfo; i!=NULL; i=i->ai_next){
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
            continue;
        }   
 
        if (bind(sockfd, i->ai_addr, i->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }
    if (listen(sockfd, 10) == -1) {
        signal_handler(1);
    }    

	freeaddrinfo(serverinfo);   
    //free(i); 
	
    while(1){
        sin_size = sizeof(struct sockaddr_in);
        new_fd = accept(sockfd,(struct sockaddr*)&client_addr,(socklen_t * __restrict__)&sin_size);
        if (new_fd == -1) continue;
        pthread_t* new_connection = malloc(sizeof(pthread_t));
		queue_enqueue(&new_connection_queue,new_connection);
        pthread_create(new_connection, NULL, workthread, (void *)(&new_fd));
		//pthread_join(*new_connection,NULL);
        signal(SIGINT, signal_handler);    
    }
	
    return 0;
}

