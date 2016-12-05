#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include <ctype.h>

#define INIT_VECTOR_SIZE 100

typedef struct{
  int *data;
  int size;
  int capacity;
} vector;

typedef struct{
  int num;
  int size;
} mode_hold;

void parse_getc(FILE *fp);
vector* init_vector(void);
void insert_element_vector(vector *v, int element_to_insert);
void print_vector(vector *v);
void free_vector(vector *v);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);
mode_hold find_mode(int arr[], int arr_size); 
mode_hold find_mode2(int arr[], int arr_size, mode_hold temp);

int main(int argc, char** argv){

  if(argc!=2){
    exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
  }else{
        
	FILE *fp = NULL;
	fp =fopen(argv[1], "r");

	if(fp==NULL){
	  exit(FILE_FAILED_TO_OPEN);
	}

	parse_getc(fp);

	int cl = fclose(fp); // int to see if file closed correctly
	
	if(cl != 0){
	  exit(FILE_FAILED_TO_CLOSE);
	}
  }
 return 0;
}



void parse_getc(FILE* fp) {
	char c;
	char* value = NULL;
	size_t val_size = 0;
        int val;
	int empty = 1;

	vector *v = init_vector();
        
	while ((c = getc(fp)) != EOF) {
		empty = 0;
                value = realloc(value, sizeof(char)*(++val_size));
                if (value == NULL)
                { exit(-2);
                }
		else if (isdigit(c)) {
			value[val_size-1] = c;
			empty=1;

		}
		else if (isspace(c) && (int) val_size > 1) {
			value[val_size-1] = '\0';
			val = atoi(value);
			insert_element_vector(v, val);
			free(value);
                        value = NULL;
			val_size = 0;
		}
		else if(getc(fp) == EOF){
			 free(value);
			 break;
		}
		else{
			free(value);
			free_vector(v);
			fclose(fp);
			exit(PARSING_ERROR_INVALID_CHARACTER_ENCOUNTERED);
		}
	}

	// Makes sure there was something in file first
	if(empty==1){
	  free_vector(v);
	  exit(PARSING_ERROR_EMPTY_FILE);
	}

	// If less than 3 elements, print default 0 (no need to go further)	
	if(v->size < 3){
	  printf("0\n");
	  free_vector(v);
	}
	else{
	  mergeSort(v->data, 0, v->size-1);
	  mode_hold temp = find_mode(v->data, v->size);
	  mode_hold temp_2 = find_mode2(v->data, v->size, temp);
	  printf("%d\n",temp_2.num); 	
	  //print_vector(v);
	  free_vector(v);
	}
}


vector* init_vector(void)
{	
		
	vector *v = malloc(sizeof(vector));
	v->data = malloc(sizeof(int) * INIT_VECTOR_SIZE);
	v->size = 0;
	v->capacity = INIT_VECTOR_SIZE;
}


void insert_element_vector(vector *v, int element_to_insert)
{
	if(v->capacity == v->size)
	{
		v->data = realloc(v->data, sizeof(int) * v->capacity * 2);
		v->capacity *= 2;
	}
	v->data[v->size] = element_to_insert;
	v->size += 1;
}


void print_vector(vector *v){
  int i=0;
  for(i=0; i < v->size; i++){
	printf("%d\n", v->data[i]);
  }
  return;
}


void free_vector(vector *v)
{
	free(v->data);
	free(v);
	v->size = 0;
}


// Merge Sort modified from: http://quiz.geeksforgeeks.org/merge-sort/
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1; //n1 = number of elements in first array
    int n2 =  r - m; // n2 = number of elements in second array
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];  // Store values from main array into LEFT
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j]; // Store values from main array into RIGHT
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}
 
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;
 
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);
 
        merge(arr, l, m, r);
    }
}

mode_hold find_mode(int arr[], int arr_size){
  int num=arr[0], prev_num=num, mode=1, i, count=1;
  mode_hold temp;
  
  // We go through every number in array once
  for(i=1; i<arr_size; i++){
    if(arr[i] == prev_num){ 
      count++;// Increase count by 1 if number is recurring
      if(i+1<arr_size) continue; // skip to next iteration if not end of array
    }

    if(count > mode){
      mode = count;
      num = prev_num;
    }

  prev_num = arr[i];
  count=1;

  }

  temp.num = num;
  temp.size = mode;
  return temp;
}
 
mode_hold find_mode2(int arr[], int arr_size, mode_hold temp){
  int num=arr[0], prev_num=num, mode=1, i, count=1;
  mode_hold temp_2;
  temp_2.num=0;
  temp_2.size=mode;


  // Special case for 1st number if only recurring once
  if(arr[0] != arr[1]){
    temp_2.num = prev_num;
  }

  for(i=1; i<arr_size; i++){
    if(arr[i] == prev_num){
	count++;
        if(i+1<arr_size)  continue;
	
    }

    if((count<temp.size) && (count>=mode)){
	temp_2.num = prev_num;
	temp_2.size = count;
	mode = count;
    }
    
    prev_num = arr[i];
    count=1;


    // Special condition for last element
    if(i+1 == arr_size && arr[i] != arr[i-1]){
	if(count<temp.size && count>=mode){
	  temp_2.num = prev_num;
	  temp_2.size = count;
	}
    }
  }

  if(temp_2.size == temp.size){
    temp_2.num = 0;
  }

  return temp_2;
}

