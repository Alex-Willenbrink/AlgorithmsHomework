#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "input_error.h"

typedef struct{
  int src, dest, weight, used;
} Edge;

typedef struct{
  int V, E;
  Edge *edge;
} Graph;

typedef struct{
  int parent, rank;
} Subset; 

typedef struct{
  Subset *subset;
  Graph *graph;
} Temp;
  
Temp parse(FILE *fp);
Edge process(char *value, int val_size);
Subset* init_Subset(int verts);
void print_edges(Temp);
void merge(Edge edges[], int l, int m, int r);
void mergeSort(Edge edges[], int l, int r);
int find(Subset subset[], int i);
void Union(Subset subset[], int x, int y);
void free_all(Temp temp);

int main(int argc, char** argv){

  if(argc!=3){
    exit(INCORRECT_NUMBER_OF_COMMAND_LINE_ARGUMENTS);
  }

  // Initialize pointers for files and open both
  FILE *fp_i = NULL;
  FILE *fp_o = NULL;
    
  fp_i = fopen(argv[1], "r");
  fp_o = fopen(argv[2], "w");

  if((fp_i==NULL) || (fp_o==NULL)){
    exit(FILE_FAILED_TO_OPEN);
  }

  // Parse file info first
  Temp temp = parse(fp_i);

  if(fclose(fp_i)!=0){
    exit(FILE_FAILED_TO_CLOSE);
  }
 
  // Sort the edges by weight
  mergeSort(temp.graph->edge, 0, temp.graph->E-1);
  
  Graph *graph = temp.graph;
  Edge *edges = graph->edge;
  
  // Make our MST
  int i, j;
  for(i=0; i<graph->E; i++){
    if(find(temp.subset, edges[i].src-1) == find(temp.subset, edges[i].dest-1)){
	continue;
    }else{
	Union(temp.subset, edges[i].src-1, edges[i].dest-1);
	edges[i].used = 1;
    }
  }
    
  // A for loop to make sure we have 1 continuous tree
  int mst = 1; 
  for(i=0; i<graph->V-1; i++){
    if(find(temp.subset, i) == find(temp.subset, i+1)){
	continue;
    }else{
	mst = 0;
	break;
    }
  }
  
  // If not 1 continuous tree, print 0, otherwise print vertices
  if(mst==0){
    fprintf(fp_o, "0\n");
  }else{
    for(i=0; i<graph->E; i++){
	if(edges[i].used == 1){
	  fprintf(fp_o, "(%d,%d)\n", edges[i].src, edges[i].dest);
	}
    }
  }

  // Close output file and free allocated space
  if(fclose(fp_o)!=0){
    exit(FILE_FAILED_TO_CLOSE);
  }

  free_all(temp);
  return 0;
}


// Function for freeing allocated space
void free_all(Temp temp){
  int i;
  Edge* edge = temp.graph->edge;
  Subset* subset = temp.subset;

  free(edge);
  free(subset);
  free(temp.graph);
}

// Function to add edges to disjoint connections 
void Union(Subset subset[], int x, int y){
  // x and y are reference positions
  int xroot = find(subset, x);
  int yroot = find(subset, y);

  // When making union, set higher ranking vertice as root
  if(subset[xroot-1].rank < subset[yroot-1].rank){
    subset[xroot-1].parent = yroot;
    subset[yroot-1].rank++;
  }else{
    subset[yroot-1].parent = xroot;
    subset[xroot-1].rank++;
  }
  return;
}

// Helper function, reduces time complexity and finds parent of 'i'
int find(Subset subset[], int i){
  // i in this instance refers to reference position
  // Use recursion to make all vertices in path directly connected to root
  if(subset[i].parent != i+1){
    subset[i].parent = find(subset, subset[i].parent-1);
  }

  return subset[i].parent;
}

// Helper function for mergeSort
void merge(Edge edges[], int l, int m, int r){
  int i, j, k;
  int n1 = m-l+1;
  int n2 = r-m;

  Edge L[n1], R[n2];

  for(i=0; i<n1; i++)
    L[i] = edges[l+i];
  for(j=0; j<n2; j++)
    R[j] = edges[m+1+j];

  i=0;
  j=0;
  k=l;

  while(i<n1 && j<n2){
    if(L[i].weight <= R[j].weight){
	edges[k] = L[i];
	i++;
    }else{
	edges[k] = R[j];
	j++;
    }
    k++;
  }

  while(i<n1){
    edges[k] = L[i];
    i++;
    k++;
  }

  while(j<n2){
    edges[k] = R[j];
    j++;
    k++;
  }
}

// Sorting function to sort weights of edges
void mergeSort(Edge edges[], int l, int r){
  if(l<r){
    int m = l+(r-l)/2;
    mergeSort(edges, l, m);
    mergeSort(edges, m+1, r);

    merge(edges, l, m, r);
  }
}

// Sub function of Parse for edge fabrication and processing
Edge process(char *value, int val_size){
  // Initialize temporary storage units and 1st character checker
  char v1[10], v2[10], w[10];
  int i;
  for(i=0; i<10; i++){
    v1[i] = '\0'; 
    v2[i] = '\0';
    w[i] = '\0';
  }

  int count = 1;
  Edge edge;
  i = 0;

  if(value[0] != '(') exit(PARSING_ERROR_INVALID_FORMAT);
  while(isdigit(value[count])){
    v1[i++] = value[count++];
  }
  
  if(v1[0] == '\0') exit(PARSING_ERROR_INVALID_FORMAT);
 
  if(value[count] != ','){
    exit(PARSING_ERROR_INVALID_FORMAT);
  }else{count++;}

  i = 0; // Put 'i' back to 0
  while(isdigit(value[count])){
    v2[i++] = value[count++];
  }
  
  if(v2[0] == '\0') exit(PARSING_ERROR_INVALID_FORMAT); 
 
  if(value[count] != ','){
    exit(PARSING_ERROR_INVALID_FORMAT); 
  }else{count++;}
 
  i = 0;
  while(isdigit(value[count])){
    w[i++] = value[count++];
  }
 
  if(w[0] == '\0') exit(PARSING_ERROR_INVALID_FORMAT);
 
  if(value[count] != ')') exit(PARSING_ERROR_INVALID_FORMAT);
 
  edge.src = atoi(v1);
  edge.dest = atoi(v2);
  edge.weight = atoi(w);
  edge.used = 0;
  return edge;
}    

// Main parsing function
Temp parse(FILE *fp){
  char c;
  char* value = NULL;
  size_t val_size = 0;
  int verts, edges=0;
  Temp temp;

  // Special first while loop to find number of vertices in graph
  while((c=getc(fp)) != '\n' ){
    if(!(isdigit(c))) exit(PARSING_ERROR_EMPTY_FILE); // either EOF or not digit
    value = realloc(value, sizeof(char)*(++val_size));
    value[val_size-1] = c;
  }
  
  verts = atoi(value);
  val_size = 0;
  free(value);
  value = NULL;

  if(verts < 1) exit(PARSING_ERROR_EMPTY_FILE);
  Subset *subset = init_Subset(verts); // Initialize subset
  
  // Initialize Graph
  Graph *graph = malloc(sizeof(Graph));
  graph->V = verts;
  graph->E = 0; // Find this out after processing incoming info
  graph->edge = NULL;

  // Main parsing for edges  
  while((c=getc(fp)) != EOF){
    if((c!='\n') && (c!=EOF)){
	value = realloc(value, sizeof(char)*(++val_size));
	value[val_size-1] = c;
    }else{
	Edge edge = process(value, val_size);

	// Make sure source within vertice bounds
	if((edge.src>graph->V) || (edge.src<1)) exit(INTEGER_IS_NOT_A_VERTEX);
	
	// Make sure destination within verice bounds
	if((edge.dest>graph->V) || (edge.dest<1)) exit(INTEGER_IS_NOT_A_VERTEX);

	// Add the edge to the graph
	graph->edge = realloc(graph->edge, sizeof(Edge)*(++edges));
	graph->edge[edges-1] = edge;	
	free(value);
	value = NULL;
	val_size = 0;
    }

    if(c == EOF) break; // Have reached end of input file
  }

    if(edges==0) exit(1); // No edges for a graph
    graph->E = edges;
    temp.subset = subset;
    temp.graph = graph;
      
    return temp;
}

// Function to inititalize the subset structure
Subset* init_Subset(int verts){
  int i;
  
  Subset *subset = malloc(sizeof(Subset)*verts);

  for(i=0; i<verts; i++){
    subset[i].parent = i+1;
    subset[i].rank = 0;
  }
  return subset;
}

// Extra function if we want to print edges of graph
void print_edges(Temp temp){
  int i;
  Graph *graph = temp.graph;
  Edge *edge = graph->edge;
  for(i=0; i<graph->E; i++){
	printf("edge %d, src %d, dest %d, weight %d\n",i, edge[i].src, edge[i].dest, edge[i].weight);
  }
}

