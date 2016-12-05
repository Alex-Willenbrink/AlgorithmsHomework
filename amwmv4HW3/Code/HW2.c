#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "input_error.h"

// struct for intitial vertices storing
typedef struct {
  int indegree;
  int* outdegree;
  int od_count;
} vert_a;  

// struct for queue
typedef struct q_ {
  int* outdegree;
  int id;
  struct q_* nxtvert;
} vert_q;

// struct to store front and rear of queue
typedef struct {
  vert_q *front;
  vert_q *rear;
} vert_info;
  

void store_info(char line[], char str1[], char str2[], vert_a array[], size_t tot_verts);
void parse_getc(FILE *fp_i, FILE *fp_o);
/* 
   So parse_getc ended up being my main function more or less. It implements
   all the topological sorting within it along with the necessary parsing
   and most of the error checking.
*/

vert_a* init_vert_a(int verts);
void vert_enqueue(vert_info *info, vert_q *new);
//void vert_queue_list(vert_info *info); //OPTIONAL FUNCTION
void vert_dequeue(vert_info *info, vert_a array[], int fin_array[], int* fin_count);
 
int main(int argc, char** argv){
  if(argc!=3){ 
    exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
  }else{
    FILE *fp_i = NULL;
    FILE *fp_o = NULL;
    fp_i = fopen(argv[1], "r");
    fp_o = fopen(argv[2], "w");
	if((fp_i==NULL) || (fp_o==NULL)){
	    exit(FILE_FAILED_TO_OPEN);
	}
    parse_getc(fp_i, fp_o);
  }
  return 0;
}

void parse_getc(FILE *fp_i, FILE *fp_o){
  char c, str1[10], str2[10];
  char* value = NULL;
  size_t val_size = 0, tot_verts;
  int i;
  
  
  // Special first while loop to find number of vertices in graph
  while((c=getc(fp_i)) != '\n'){ 
    if(!(isdigit(c))) exit(PARSING_ERROR_INVALID_FORMAT);
    value = realloc(value, sizeof(char)*(++val_size));
    value[val_size-1] = c;
  }

  tot_verts = atoi(value);

  if(tot_verts < 1) exit(PARSING_ERROR_EMPTY_FILE);
  
  vert_a *array = init_vert_a(tot_verts);

  // Reset values for reading other stuff
  free(value);
  value = NULL;
  val_size = 0;
 
  // While loop for parsing every other line in input file
  while((c=getc(fp_i)) != EOF){
    value = realloc(value, sizeof(char)*(++val_size));
    
    if(isdigit(c)){
	value[val_size-1] = c;
    }else if(c != '\n'){
	value[val_size-1] = c;
    }else if(c == '\n'){
	// Resets numbers str1 and str2
	for(i=0; i<10; i++){
	  str1[i] = '\0';
	  str2[i] = '\0';
	}

	store_info(value, str1, str2, array, tot_verts);
	free(value);
	value = NULL;
	val_size = 0;
    
    }else if(getc(fp_i) == EOF){
	// Resets numbers str1 and str2
	for(i=0; i<10; i++){
	  str1[i] = '\0';
	  str2[i] = '\0';
	}

	store_info(value, str1, str2, array, tot_verts);
	free(value);
	
	break;

    }else{
	free(value);
	break;
    }	
  }
  
  // Close input file
  int closed = fclose(fp_i);
  if(closed!=0) exit(FILE_FAILED_TO_CLOSE);

  // DONE PARSING DATA AND STORING INFO, NOW WE PROCESS DATA 

  // Initialize info_array structure (will store front and back of queue)
  vert_info *info = malloc(sizeof(vert_info));
  info->front = NULL;
  info->rear = NULL;
  
  // Add vertices to queue with no indegrees
  for(i=0; i<tot_verts; i++){
    if(array[i].indegree == 0){
	// Initialize new vertex to be enqueued
	vert_q *vert_new = malloc(sizeof(vert_q));
	vert_new->nxtvert = NULL;
	vert_new->id = i+1; 
	
	vert_enqueue(info, vert_new);
    }
  }
  //vert_queue_list(info);
  //Make array for storing ids
  int fin_array[tot_verts], fin_count[1];
  fin_count[0] = 0;
  
  // Keep enqueueing data until all values have been looked at
  while(info->front != NULL){
    vert_dequeue(info, array, fin_array, fin_count);
  }

  /* (OPTIONAL FUNCTION)
  Show what's in array storage id
  for(i=0; i<fin_count[0]; i++){
	printf("%d ", fin_array[i]);
  }
  printf("\n"); 
  */
  
  /* Double check to make sure all edges were only processed once. If they 
     were processed once, their indegree value will be -1. If they were were
     processed multiple times, it will be less than -1
  */

  int ind_count = 0;
  for(i=0; i<fin_count[0]; i++){
	if(array[i].indegree==-1){
	   ind_count++;
 	}
  }
  // Paste data to output file
  if(ind_count==fin_count[0]){
    for(i=0; i<fin_count[0]; i++){
	  fprintf(fp_o, "%d\n",fin_array[i]);
    }
  }else{
	fprintf(fp_o, "0");
  }  
  
  // Free All info 
  free(info);
  for(i=0; i<tot_verts; i++){
	if(array[i].od_count>0){
	  free(array[i].outdegree);
	}
  }
  free(array);
 
  // Close output file
  closed = fclose(fp_o);
  if(closed!=0) exit(FILE_FAILED_TO_CLOSE);
  
  return;
}

void store_info(char line[],char str1[], char str2[], vert_a array[],size_t tot_verts){
  int count_a = 0, count_a2;

// PROCESS NUM 1 PARSING
  if(line[0] != '(') exit(PARSING_ERROR_INVALID_FORMAT);
  while(isdigit(line[++count_a]));
  if(count_a==1) exit(PARSING_ERROR_INVALID_FORMAT);
  strncpy(str1, line+1 , count_a-1);

// PROCESS NUM 2 PARSING
  count_a2 = count_a+1;
  if(line[count_a] != ',') exit(PARSING_ERROR_INVALID_FORMAT);
  while(isdigit(line[++count_a]));
  if(count_a-count_a2 == 0) exit(PARSING_ERROR_INVALID_FORMAT);
  strncpy(str2, line+count_a2, count_a-count_a2);
  if(line[count_a] != ')') exit(PARSING_ERROR_INVALID_FORMAT);   
  
  
  // Initialize both numbers to make sure within acceptable range
  int num1 = (int)atoi(str1), num2 = (int)atoi(str2);

  if((num1<1) || (num1>tot_verts) || (num2<1) || (num2>tot_verts)){
	exit(INTEGER_IS_NOT_A_VERTEX);
  }

  // Add info for each array for outdegree
  array[num1-1].od_count++;
  int od_count = array[num1-1].od_count;
  array[num1-1].outdegree = realloc(array[num1-1].outdegree, sizeof(int)*od_count);
  array[num1-1].outdegree[od_count-1] = num2;
  // for num2 (ending vertice)
  array[num2-1].indegree++;
  return;
}

// Initializes array for storing all vertice and vertice info
vert_a* init_vert_a(int verts){
  vert_a *array = malloc(sizeof(vert_a)*verts);
  int i;

  // Sets all indegrees equal to 0 to start and outdegree_count (od_count)
  for(i=0; i<verts; i++){
    (array+i)->indegree = 0;
    (array+i)->od_count = 0;
    //(array+i)->outdegree = malloc(sizeof(int)); ////////////
  }
  return array;
}

void vert_enqueue(vert_info *info, vert_q *new){

  // Basically just adding the vertices
  if(info->front == NULL){
    info->front = new;
  }else if(info->rear == NULL){
    info->front->nxtvert = new;
    info->rear = new;
  }else{
    info->rear->nxtvert = new;
    info->rear = new;
  }
  return;

}

/* (OPTIONAL FUNCTION)
// Lists queues
void vert_queue_list(vert_info *info){
  if(info->front == NULL){
    return;
  }else{
    vert_q *vert = info->front;
    while(vert!=NULL){
      vert = vert->nxtvert;
    }
  }

  return;
}

*/
void vert_dequeue(vert_info *info, vert_a array[], int fin_array[], int* fin_count){
  int i, id, out_curr;
  if(info->front == NULL){
    return;
  }

  vert_q *vert_curr = info->front;
  id = vert_curr->id;
  array[id-1].indegree--; // Make indegree for vertice in array = -1

  for(i=0; i<array[id-1].od_count; i++){
    out_curr = array[id-1].outdegree[i];
    array[out_curr-1].indegree--; // Make indegree of outdegrees -1

    if(array[out_curr-1].indegree==0){ // if indegrees are now = 0 add to queue
	// make new ver_q
	vert_q *vert_new = malloc(sizeof(vert_q));
	vert_new->nxtvert = NULL;
	vert_new->id = out_curr;
	vert_enqueue(info, vert_new);
    }
  } 
  
    // Store id of vertex for later output
    fin_array[++fin_count[0]-1] = id;

    // 1st situation (1 item in queue left)
    if(info->rear == NULL){
	free(info->front);
	info->front = NULL;
    }
    // 2nd Situation (2 items in queue left)
    else if(info->front->nxtvert == info->rear){
	//printf("checkpoint 2\n");
	free(info->front);
	info->front = info->rear;
	info->rear = NULL;
    }
    // 3rd (final) Situation (3 or more items in queue left)
    else{
	vert_q *temp = info->front->nxtvert;
	free(info->front);
	info->front = temp;
    }

  return;
}



