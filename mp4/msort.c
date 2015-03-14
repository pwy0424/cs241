/** @file msort.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*typedef struct _split_struct{
	int size;
	int * array_pointer;
}split_struct;*/

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void* sort(void* numbers)
{
	
	fprintf(stderr, "Sorted %d elements.\n", ((int*)numbers)[0]);
	qsort(numbers+sizeof(int), ((int*)numbers)[0], sizeof(int), compare);
	//printf("?");
	//int i = 0;
	//printf("size:%d\n", (*(int**)numbers)[0]);
	return NULL;
}

void* merge(void* numbers)
{
	int* num1 = ((int**)numbers)[0];
	int* num2 = ((int**)numbers)[1];
	int dupes = 0;
	if(num1 == NULL && num2 == NULL) return NULL;
	else if(num1 == NULL && num2 != NULL) return num2;
	else if(num1 != NULL && num2 == NULL) return num1;
	else
	{
		int size = num1[0] + num2[0];
		int sizej = num1[0];
		int sizek = num2[0];
		int* result = malloc((size+1)*sizeof(int));
		result[0] = size;
		int i;
		int j = 1;
		int k = 1;
		for(i = 1; i < size + 1; i++)
		{
			if((j > sizej) && (k <= sizek))
			{
				result[i] = num2[k];
				k++;
			}
			else if((k > sizek) && (j <= sizej))
			{
				result[i] = num1[j];
				j++;
			}
			else if((k <= sizek) && (j <= sizej))
			{
				if(num1[j] == num2[k])
				{
					dupes++;
					result[i] = num1[j];
					j++;
				}
				else if(num1[j] > num2[k])
				{
					result[i] = num2[k];
					k++;
				}
				else if(num1[j] < num2[k])
				{
					result[i] = num1[j];
					j++;
				}
			}
		}
		fprintf(stderr, "Merged %d and %d elements with %d duplicates.\n", sizej, sizek, dupes);
		free(num1);
		free(num2);
		//free(numbers);
		numbers = NULL;
		return result;
	}
}

int main(int argc, char **argv)
{
	int* long_array = malloc(1024*sizeof(int));
	int length = 1024;
	int i = 0;
	
	//FILE * inputfile;
	//inputfile = 
	char* input = NULL;
	size_t size;
	getline(&input, &size, stdin);
	input[strlen(input)-1] = '\0';
	long_array[i] = atoi(input);
	i++;
	while(input != NULL)
	{
		if(i == length)
		{
			long_array = realloc(long_array,length*2*sizeof(int));
			length = length*2;
		}
		int error = getline(&input, &size, stdin);
		if(error == -1) break;
		input[strlen(input)-1] = '\0';
		long_array[i] = atoi(input);
		i++;
		//printf("%d\n",atoi(input));
		
	}

		
	
	//printf("check point 1\n");

	int segment_count = atoi(argv[argc-1]);
	
	int values_per_segment; //< Maximum number of values per segment. 

	if (i % segment_count == 0)
    	values_per_segment = i / segment_count;
	else
		values_per_segment = (i / segment_count) + 1;
	
	//printf("check point 2\n");

	int** split_array = malloc(segment_count*sizeof(int*));//[values_per_segment+1];
	int a = 0;
	for(a = 0; a < segment_count;a++)
	{
		split_array[a] = malloc((values_per_segment+1)*sizeof(int));
	}

	int j = 0;
	int k = 0;
	int l = 0;
	//split_struct split_pass[segment_count];
	
	while(l < i)
	{
		if(k == 0)
		{
			//printf("l = %d\n",l);
			if((i-l) > values_per_segment) split_array[j][k] = values_per_segment;
			else split_array[j][k] = i - l;
			//printf("split_array[%d][%d] = %d\n", j, k, split_array[j][k]);
			k++;
		}
		split_array[j][k] = long_array[l];
		//printf("split_array[%d][%d] = %d\n", j, k, split_array[j][k]);
		k++;
		l++;
		if(k == values_per_segment+1)
		{
			k = 0;
			j++;
		}
	}
	//split_pass[j].array_pointer = split_array[j];
	//split_pass[j].size = k;
	
	int z = 0;
	pthread_t* tid1 = malloc(segment_count*sizeof(pthread_t));
	//pthread_create(&tid[z], NULL, sort, &split_array[z]);
	//printf("%d\n",split_array[0][x]);
	for(z = 0; z < segment_count; z++)
	{
		//printf("begin sort\n");
		//printf("%d\n",split_array[z][0]);
		pthread_create(&tid1[z], NULL, sort, split_array[z]);
		
		//sort(&split_array[z]);
	}
	for(z = 0; z < segment_count; z++)
	{
		pthread_join(tid1[z], NULL );
	}

	free(tid1);
	free(long_array);
	free(input);
	/*int x;
	for(x = 0; x < 6; x++)
	{
		printf("%d\n",split_array[0][x]);
	}*/
	
	int mergecount = segment_count;
	pthread_t* tid2;// = malloc((mergecount - mergecount/2)*sizeof(pthread_t));
	while(mergecount > 1)
	{
		tid2 = malloc((mergecount - mergecount/2)*sizeof(pthread_t));
		int ** temp_result = malloc(sizeof(int*)*(mergecount - (mergecount/2)));
		int *** temp_input = malloc((mergecount - mergecount/2)*sizeof(int**));
		int q;
		for(q = 0; q < (mergecount - mergecount/2); q++)
		{
			temp_input[q] = malloc(2*sizeof(int*));
			temp_input[q][0] = split_array[2*q];
			if(2*q+1 >= mergecount) temp_input[q][1] = NULL;
			else temp_input[q][1] = split_array[2*q + 1];
			pthread_create(&tid2[q], NULL, merge, temp_input[q]);
			//if(temp_input != NULL) free(temp_input);
		}
		for(q = 0; q < (mergecount - mergecount/2); q++)
		{
			pthread_join(tid2[q],(void **) &(temp_result[q]));
		}
		//************************FREE!!*******************	

		for(q = 0; q < (mergecount - mergecount/2); q++)
		{
			free(temp_input[q]);
		}
		free(temp_input);

		free(split_array); 
		//***************************************************************
		split_array = malloc(sizeof(int*)*(mergecount - (mergecount/2)));
		for(q = 0; q < (mergecount - mergecount/2); q++)
		{
			split_array[q] = temp_result[q];
			//free(temp_result[q]);
		}

		free(temp_result);
		//free(temp_input);
		mergecount = mergecount - mergecount/2;
		free(tid2);
	}

	//printf("yeah!!!");

	
	int x;
	for(x = 1; x < i+1; x++)
	{
		printf("%d\n",split_array[0][x]);
	}
	free(split_array[0]);
	free(split_array);
	//free(tid2);
	return 0;
}
