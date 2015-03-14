/** @file part1.c */

/*
 * Machine Problem #1
 * CS 241 Fall2013
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mp1-functions.h"

/**
 * (Edit this function to print out the ten "Illinois" lines in mp1-functions.c in order.)
 */
int main()
{
	//1
	first_step(81);

	//2
	int* second = malloc(sizeof(int));
	*second = 132;
	second_step(second);
	free(second);
	
	//3
	int** third = NULL;
	int* third2 = malloc(sizeof(int));
	*third2 = 8942;
	third = &third2;
	double_step(third);
	free(third2);

	//4
	int* fourth = NULL;
	strange_step(fourth);

	//5
	char* fifth = malloc(4*sizeof(char));	
	fifth[3] = 0;
	empty_step(fifth);
	free(fifth);

	//6
	char* sixth = malloc(4*sizeof(char));	
	sixth[3] = 'u';
	two_step(sixth,sixth);
	free(sixth);

	//7
	char* seventh = malloc(5*sizeof(char));
	three_step(seventh,seventh+2,seventh+4);
	free(seventh);

	//8
	char* eighth1 = malloc(2*sizeof(char));
	eighth1[1] = 0;
	char* eighth2 = malloc(3*sizeof(char));
	eighth2[2] = 8;
	char* eighth3 = malloc(4*sizeof(char));
	eighth3[3] = 16;

	step_step_step(eighth1,eighth2,eighth3);
	free(eighth1);
	free(eighth2);
	free(eighth3);

	//9
	char* nineth = malloc(sizeof(char));
	*nineth = 1;
	it_may_be_odd(nineth,1);
	free(nineth);

	//10
	char* tenth = malloc(4*sizeof(char));
	tenth[0] = 1;
	tenth[1] = 0;
	tenth[2] = 2;
	tenth[3] = 0;
	the_end(tenth,tenth);
	free(tenth);	

	return 0;
}
