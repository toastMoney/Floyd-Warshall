#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct graph_struct_{
//	int vertices;
//	int processes;
//	int** matrix;
//};
//typedef struct graph_struct_ graph_struct;
int num_vertices, num_processes;

int** initialize_graph(num_vertices){
		int i,j;

		int** ret = malloc(sizeof(int*) * num_vertices);
		for(i = 0; i<num_vertices; i++){
				ret[i] = malloc(sizeof(int) * num_vertices);
		}

		for(i=0; i<num_vertices; i++){
				for(j=0; j<num_vertices; j++){
						if(i == j){
								ret[i][j] = 1;
						}
						else{
								ret[i][j] = 0;
						}
				}
		}
		int k,l;
		for(k=0; k<num_vertices; k++){
				for(l=0; l<num_vertices; l++){
						printf("%d",ret[k][l]);
				}
				printf("\n");
		}
		return ret;
}

int** build_graph(){
		FILE *fp;
		char *line = NULL;
		size_t len = 0;
		ssize_t read;
		const char* delim = "\n";
		char *token = NULL;
		int **graph = NULL;
		int count;

		fp = fopen("graph1.txt", "r");
		if (fp == NULL){
				return 0;
		}

		while ((read = getline(&line, &len, fp)) != -1) {
				token = strtok(line, delim);
				if(strlen(token) == 1){
						printf("token integer %d\n",atoi(token));
						if(count == 0){
								num_processes = atoi(token);
								count++;
						}
						else{
								num_vertices = atoi(token);
								graph = initialize_graph(num_vertices);
						}
						token = strtok(NULL,delim);
				}
				else{
						printf("token %s\n",token);
						int index1 = atoi(token)-1;
						int index2 = atoi(token+2)-1;
						graph[index1][index2] = 1;
						printf("Index 1: %d\n",index1);
						printf("Index 2: %d\n",index2);
						printf("index of graph is %d\n",graph[index1][index2]);
						token = strtok(NULL,delim);
				}
		}

		int k,l;
		printf("\n\ngraph after initialization: \n");
		for(k=0; k<num_vertices; k++){
				for(l=0; l<num_vertices; l++){
						printf("%d",graph[k][l]);
				}
				printf("\n");
		}


		if (line){
				free(line);
		}
		return graph;
}

void build_trans_closure(){

		int** result_graph;
		result_graph = build_graph();

		int i, j, k ,o, p;


		for(k = 0; k < num_vertices; k++){
				for(i = 0; i < num_vertices; i++){
						for(j = 0; j < num_vertices; j++){
								if(result_graph[i][k] == 1 && result_graph[k][j] == 1)
										result_graph[i][j] = 1;
						}
				}
		}
		
		printf("\n\ngraph after transitive closure:\n");
		for(o=0; o<num_vertices; o++){
				for(p=0; p<num_vertices; p++){
						printf("%d",result_graph[o][p]);
				}
				printf("\n");
		}
}

void main(){
		//int** graph1; 
		//graph1 = build_graph();
		build_graph();
		build_trans_closure();

}
