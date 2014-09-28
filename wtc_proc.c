#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **initialize_graph(num_vertices){
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

float build_graph(){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = "\n";
    char *token = NULL;
    int **graph = NULL;
    int count;
    int process_count;
    int num_vertices;

    fp = fopen("graph1.txt", "r");
    if (fp == NULL){
        return 0;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strlen(token) == 1){
            printf("token integer %d\n",atoi(token));
            if(count == 0){
                process_count = atoi(token);
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
            int index1 = atof(token)-1;
            int index2 = atoi(token+2)-1;
            graph[index1][index2] = 1;
            printf("Index 1: %d\n",index1);
            printf("Index 2: %d\n",index2);
            printf("index of graph is %d\n",graph[index1][index2]);
            token = strtok(NULL,delim);
        }
    }

    int k,l;
    for(k=0; k<num_vertices; k++){
        for(l=0; l<num_vertices; l++){
            printf("%d",graph[k][l]);
        }
        printf("\n");
    }

    if (line){
        free(line);
        return 0;
    }
}

void main(){
    build_graph();
}
