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

void print_graph(void);

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

//void *build_trans_closure(){
//
//    int** result_graph;
//    result_graph = build_graph();
//
//    int i, j, k;
//    for(k = 0; k < num_vertices; k++){
//        for(i = 0; i < num_vertices; i++){
//            for(j = 0; j < num_vertices; j++){
//                if(result_graph[i][k] == 1 && result_graph[k][j] == 1)
//                    result_graph[i][j] = 1;
//            }
//        }
//    }
//
//    printf("\n\ngraph after transitive closure:\n");
//    print_graph(result_graph);
//    printf("Num vertices %d\n",num_vertices);
//    printf("Num processes: %d\n",num_processes);
//    return NULL;
//}

//void main(){
//    //int** graph1;
//    //graph1 = build_graph();
//    build_graph();
//    build_trans_closure();

int main(int argc, char **argv) {
    key_t shmkey;
    pid_t pid;
    int n;
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
    printf ("p=%d is allocated in shared memory.\n\n", *graph);
    return 0;
}

