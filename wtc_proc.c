#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct range_{
    int start;
    int end;
};
typedef struct range_ range;

int num_vertices, num_threads, mod_count, position, vert_per_thread, global_start, global_end;

int** graph;

range* create_range(){
    range *temp_range = malloc(sizeof(range));
    temp_range->start = 0;
    temp_range->end = 0;
    position = 0;
    return temp_range;
}

range* set_range(range* arg_range){

    if(arg_range->end == 0){
        arg_range->start = 0;
        arg_range->end = (arg_range->start + vert_per_thread - 1);
        position = arg_range->end;

        printf("check range #1// start: %d\t\tend: %d\n", arg_range->start, arg_range->end);
        if(mod_count > 0){
            arg_range->end++;
            mod_count--;
            position = arg_range->end;
        }
    printf("check range #2// start: %d\t\tend: %d\n", arg_range->start, arg_range->end);
    return arg_range;
    }

    else{
        printf("check range #3// start: %d\t\tend: %d\n", arg_range->start, arg_range->end);
        arg_range->start = position + 1;
        arg_range->end = (arg_range->start + vert_per_thread - 1);
        position = arg_range->end;
        if(mod_count > 0){
            arg_range->end++;
            mod_count--;
            position = arg_range->end;
        }

        printf("check range #4// start: %d\t\tend: %d\n", arg_range->start, arg_range->end);
    }


    return arg_range;
}



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
void print_graph(int** graph){
    int i,j;
    for(i=0; i<num_vertices; i++){
        for(j=0; j<num_vertices; j++){
            printf("%d",graph[i][j]);
        }
        printf("\n");
    }
}

int** build_graph(){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = "\n";
    char *token = NULL;
    graph = NULL;
    int count;
    count = 0;

    fp = fopen("graph1.txt", "r");
    if (fp == NULL){
        return 0;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strlen(token) == 1){
            printf("token integer %d\t\t count: %d\n",atoi(token), count);
            if(count == 0){
                num_threads = atoi(token);
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

    printf("\n\ngraph after initialization: \n");
    print_graph(graph);

    if (line){
        free(line);
    }
    return graph;
}

void *build_trans_closure(void* arguments){
    range *args = arguments;
    int i, j, k;
    printf("arg-start: %d\t\targ-end: %d\n\n", args->start, args->end);
    for(k = 0; k < num_vertices; k++){
        for(i = args->start; i <= args->end; i++){
            for(j = 0; j < num_vertices; j++){
                if(graph[i][k] == 1 && graph[k][j] == 1)
                    graph[i][j] = 1;
            }
        }
    }

    printf("\n\ngraph after transitive closure:\n");
    print_graph(graph);
    printf("Num vertices %d\n",num_vertices);
    printf("Num processes: %d\n",num_threads);
    return NULL;
}

//void main(){
//    //int** graph1;
//    //graph1 = build_graph();
//    build_graph();
//    build_trans_closure();

int main(int argc, char **argv) {
    build_graph();
    //printf("num_threads #1: %d\n", num_threads);
    int i, error;
    pthread_t thread_pool[num_threads]; // Array of threads
    if(num_threads)
        vert_per_thread = num_vertices/num_threads;
    //printf("num_vertices: %d\t\tnum_threads: %d\t\tvert_per_threads: %d\n", num_vertices, num_threads, vert_per_thread);

    if(num_threads == 0)
        mod_count = 0;
    else
        mod_count = num_vertices % num_threads;
    range *current;
    //printf("num_threads #2: %d\n", num_threads);

    current = create_range();
    //printf("start(fuck): %d\t\tend(fuck): %d\t\t\n", current->start, current->end);

    for (i = 0; i < num_threads; i++) {
        if(current){
            current = set_range(current);
            printf("start: %d\t\tend: %d", current->start, current->end);
        }
        else
            printf("fuck off\n");
        printf("current->start: %d\t\tcurrent->end: %d\n\n", current->start, current->end);
        error = pthread_create(&thread_pool[i], NULL, &build_trans_closure,(void*) current);
        if (error) {
            printf("Error creating phtread\n");
            return error;
        }
        printf("thread #: %d\t\tstart: %d\t\tend: %d\n\n", i, current->start, current->end);
    }

    for (i = 0; i < num_threads; i++) {
        pthread_join(thread_pool[i], NULL);
    }
    return 0;
    //return pthread_join(&thread_pool[1], NULL);

}

