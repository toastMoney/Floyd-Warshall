#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/signal.h>
#include <signal.h>

int num_vertices, num_processes, vert_per_process,mod_count, shmid;
int* graph;

void print_graph(void);

float getcpu_speed(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = ":\t";
    char * token = NULL;
    fp = fopen("/proc/cpuinfo", "r");
    if (fp == NULL){
        return 0;
    }
    //Tokenizing each line until the cpu speed is found. Once the cpu speed is found, it gets returned
    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strcmp(line, "cpu MHz") == 0){
            token = strtok(NULL," \t:");
            return atof(token);
        }
        else{
            token = strtok(NULL,delim);
        }
    }
    if (line){
        free(line);
        return 0;
    }
}

inline unsigned long long rdtsc(){
    unsigned long long cycle;
    __asm__ __volatile__("cpuid");
    __asm__ __volatile__("rdtsc" : "=r" (cycle): :);
    return cycle;
}

void initialize_graph(int num_ver){
    int i,index;
    int n = num_ver*num_ver;
    for(i=1; i<=n; i++){
        graph[i] = 0;
    }
    index = 1;
    while(index<=n){
        graph[index] = 1;
        index += num_ver+1;
    }

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

void print_result(){
    int i,j;
    int n = num_vertices*num_vertices;

    for(i=1; i<=n; i++){
        if(graph[i] == 1 ){
            if(i%num_vertices == 0){
                if(i/num_vertices != num_vertices){
                    printf("%d %d\n",i/num_vertices,num_vertices);
                }
            }
            else{
                if(i/num_vertices+1 != i%num_vertices){
                    printf("%d %d\n",i/num_vertices+1,i%num_vertices);
                }
            }
        }
    }
}
void build_graph(char* file_name){
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    const char* delim = "\n";
    char *token = NULL;
    int count;

    fp = fopen(file_name, "r");
    if (fp == NULL){
        return;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, delim);
        if(strlen(token) == 1){
            if(count == 0){
                num_processes = atoi(token);
                count++;
            }
            else{
                num_vertices = atoi(token);
                initialize_graph(num_vertices);
            }
            token = strtok(NULL,delim);
        }
        else{
            int i = atoi(token);
            int j = atoi(token+2);
            graph[(num_vertices*i+j)-num_vertices] = 1;
            token = strtok(NULL,delim);
        }
    }

    if (line){
        free(line);
    }
    return;
}

void build_trans_closure(int k, int start, int end){//,int start,int end){
    int i, j;
    for(i = start; i <= end; i++){
        for(j = 1; j <= num_vertices; j++){
            if((graph[(num_vertices*i+k)-num_vertices] == 1) & (graph[(num_vertices*k+j)-num_vertices] == 1)){
                graph[(num_vertices*i+j)-num_vertices] = 1;
            }
        }
    }
}

int main(int argc, char **argv) {
    unsigned long long time_start, time_end, cycles;
    float cpu_speed = getcpu_speed()*1000000;

    if(argc != 2){
        printf("Invalid number of arguments\n");
        printf("\nUsage: ./a.out <graph.in>\n");
        return 0;
    }
    key_t shmkey;
    int i,j,k, start, end;
    int status;

    shmkey = ftok ("/dev/null", 5);
    shmid = shmget (shmkey, sizeof(int)*(num_vertices*num_vertices+1), 0644 | IPC_CREAT);
    if (shmid < 0){
        exit (1);
    }
    graph = (int*)shmat(shmid, NULL, 0);
    build_graph(argv[1]);
    printf("\nInitial Graph after reading file\n");
    print_graph();
    pid_t pid[num_processes];
    if(num_processes)
        vert_per_process = num_vertices/num_processes;

    if(num_processes == 0)
        mod_count = 0;
    else
        mod_count = num_vertices % num_processes;
    time_start = rdtsc();
    for(k = 1; k<=num_vertices; k++){
        start = 1;
        end = 1;

        for(i = 1; i<=num_processes; i++){
            if((pid[i]= fork()) == 0){
                start = vert_per_process * i;
                end = start + vert_per_process - 1;
                if(mod_count > 0){
                    end++;
                    mod_count--;
                }
                build_trans_closure(k,(start - vert_per_process + 1), end);
                exit(EXIT_SUCCESS);
            }

        }
        for(j=0; j<num_processes; j++){
            wait(&status);
        }
    }
    time_end = rdtsc();
    cycles = time_end - time_start;

    printf("Matrix form of transitive closure: \n");
    print_graph();
    printf("\n\nEdges of transitive closure: \n");
    print_result();
    printf("\nSeconds for multi-process version(): %f\n",((cycles)/(.000001*cpu_speed)));
    printf("\nNumber of cycles for multi-process version: %d\n",cycles);
    return 0;
}
