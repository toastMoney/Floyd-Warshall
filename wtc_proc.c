#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>

int num_vertices, num_processes;
int shmid;
int* graph;

struct range_{
    int start;
    int end;
};
typedef struct range_ range;

void print_graph(void);

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
        arg_range->end = (arg_range->start + vert_per_process - 1);
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
        arg_range->end = (arg_range->start + vert_per_process - 1);
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
void initialize_graph(int num_ver){
    int i,index;
    int n = num_ver*num_ver;
    for(i=1; i<=n; i++){
        graph[i] = 0;
    }
    print_graph();
    index = 1;
    while(index<=n){
        graph[index] = 1;
        index += num_ver+1;
    }
    print_graph();

}

void print_graph(){
    int i;
    int n = num_vertices*num_vertices;
    for(i=1; i<=n; i++){
        printf("%d",graph[i]);
        if(i % num_vertices == 0){
            printf("\n");
        }
    }
    printf("\n");
}

void build_graph(){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = "\n";
    char *token = NULL;
    int count;

    fp = fopen("graph0.txt", "r");
    if (fp == NULL){
        return;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strlen(token) == 1){
            printf("token integer %d\n",atoi(token));
            if(count == 0){
                num_processes = atoi(token);
                printf("num process: %d\n",num_processes);
                count++;
            }
            else{
                num_vertices = atoi(token);
                printf("num vertices: %d\n",num_vertices);
                initialize_graph(num_vertices);
            }
            token = strtok(NULL,delim);
        }
        else{
            printf("token %s\n",token);
            int i = atoi(token);
            int j = atoi(token+2);
            printf("Index 1: %d\n",i);
            printf("Index 2: %d\n",j);
            graph[(num_vertices*i+j)-num_vertices] = 1;
            token = strtok(NULL,delim);
        }
    }

    printf("\n\ngraph after initialization: \n");
    print_graph();

    if (line){
        free(line);
    }
    return;
}

void build_trans_closure(int k,int start,int end){

    int i, j;
    for(i = start; i <= end; i++){
        for(j = 1; j <= num_vertices; j++){
            if((graph[(num_vertices*i+k)-num_vertices] = 1) && graph[(num_vertices*k+j)-num_vertices] == 1){
            graph[(num_vertices*i+j)-num_vertices] = 1;
            }
        }
    }
    printf("\n\ngraph after transitive closure:\n");
    print_graph(graph);
    printf("Num vertices %d\n",num_vertices);
    printf("Num processes: %d\n",num_processes);
}

//void main(){
//    //int** graph1;
//    //graph1 = build_graph();
//    build_graph();
//    build_trans_closure();

int main(int argc, char **argv) {
    key_t shmkey;
    pid_t pid;
    int i,j,k,vert_per_process;
    /* initialize a shared variable in shared memory */
    shmkey = ftok ("/dev/null", 5);       /* valid directory name and a number */
    printf ("shmkey for p = %d\n", shmkey);
    shmid = shmget (shmkey, sizeof(int)*(num_vertices*num_vertices+1), 0644 | IPC_CREAT);
    //shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
    printf("%d\n",shmid);
    if (shmid < 0){                           /* shared memory error check */
        //perror ("shmget\n");
        exit (1);
    }
    graph = (int*)shmat(shmid, NULL, 0);   /* attach p to shared memory */
    build_graph();
    print_graph();

    if(num_processes)
        vert_per_process = num_vertices/num_processes;

    if(num_processes == 0)
        mod_count = 0;
    else
        mod_count = num_vertices % num_processes;
    range *current;
    current = create_range();
    for(k = 1; k<=num_vertices; k++){
        current = reset_range(current);
        for(i = 0; i<num_processes; i++){
            //fork
            if(current){
                current = set_range(current);
                printf("start: %d\t\tend: %d\n", current->start, current->end);
            }
            else{
                printf("current not initialized");
            }
            printf("current->start: %d\t\tcurrent->end: %d\n\n", current->start, current->end);
            pid = fork();
            if(pid == 0){
                build_trans_closure(k,current->start,current->end);
            }
            else if (pid<0) {
                printf("failed to fork!\n");
            }
            else{
                for(j=0; j<num_processes; j++){
                    wait(0);
                }
            }
        }
    }
    return 0;
}
