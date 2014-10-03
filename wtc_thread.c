//fuck all this shit
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct range_{
    int start;
    int end;
};
typedef struct range_ range;

int num_vertices, num_threads, mod_count, position, vert_per_thread, global_k;

int** graph;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

range* create_range(){
    range *temp_range = malloc(sizeof(range));
    temp_range->start = 0;
    temp_range->end = 0;
    position = 0;
    return temp_range;
}

range* reset_range(range* arg_range){
    arg_range->start = 0;
    arg_range->end = 0;
    return arg_range;
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


void print_graph(int** graph){
    int i,j;
    for(i=0; i<num_vertices; i++){
        for(j=0; j<num_vertices; j++){
            printf("%d",graph[i][j]);
        }
        printf("\n");
    }
}

void print_result(){
    int i,j;
    for(i=0; i<num_vertices; i++){
        for(j=0; j<num_vertices; j++){
            if(graph[i][j] == 1 && i!=j){
                printf("%d %d\n",i+1,j+1);
            }
        }
    }
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
    print_graph(ret);
    return ret;
}

int** build_graph(char *file_name){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = "\n";
    char *token = NULL;
    graph = NULL;
    int count;
    count = 0;

    fp = fopen(file_name, "r");
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
    int i, j;
    printf("arg-start: %d\t\targ-end: %d\n\n", args->start, args->end);
    for(i = args->start; i <= args->end; i++){
        for(j = 0; j < num_vertices; j++){
            if(graph[i][global_k] == 1 && graph[global_k][j] == 1)
                graph[i][j] = 1;
        }
    }


    printf("\n\ngraph after transitive closure:\n");
    print_graph(graph);
    pthread_mutex_unlock(&lock);
    return NULL;
}


int main(int argc, char **argv) {
    if(argc != 2){
        printf("Invalid number of arguments\n");
        printf("\nUsage: ./a.out <graph.in>\n");
        return 0;
    }
    build_graph(argv[1]);
    int i, error, k, x, rc;
    pthread_t thread_pool[num_threads]; // Array of threads
    if(num_threads)
        vert_per_thread = num_vertices/num_threads;

    if(num_threads == 0)
        mod_count = 0;
    else
        mod_count = num_vertices % num_threads;
    range *current;

    current = create_range();

    for(global_k = 0; global_k < num_vertices; global_k++){
        current = reset_range(current);

        for (i = 0; i < num_threads; i++) {
            if(current){

                rc = pthread_mutex_lock(&lock);
                current = set_range(current);
                printf("start: %d\t\tend: %d\n", current->start, current->end);
            }
            else
                printf("current not initialized");
            printf("current->start: %d\t\tcurrent->end: %d\n\n", current->start, current->end);
            error = pthread_create(&thread_pool[i], NULL, &build_trans_closure,(void*) current);
            if (error) {
                printf("Error creating phtread\n");
                return error;
            }

        }
        printf("thread #: %d\t\tstart: %d\t\tend: %d\n\n", i, current->start, current->end);
        for(x = 0; x < num_threads; x++)
            pthread_join(thread_pool[x], NULL);
    }
    print_result();

    return 0;

}

