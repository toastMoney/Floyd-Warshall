#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float parse_graph(){
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	const char* delim = "\n";
	char * token = NULL;
	int n, p;

	fp = fopen("graph0.txt", "r");
	if (fp == NULL){
		return 0;
	}

	//Tokenizing each line until the cpu speed is found. Once the cpu speed is found, it gets returned
	while ((read = getline(&line, &len, fp)) != -1) {
		token = strtok(line, delim);
		printf("length of token %d\n",strlen(token));
		if(strlen(token) == 1){
			printf("token integer %d\n",atoi(token));
			token = strtok(NULL,delim);

		}
		else{
			printf("token %s\n",token);
			token = strtok(NULL,delim);
			n = (int)(*(token +0));
			printf("n = %d", n);
	
		}
	}

	if (line){
		free(line);
		return 0;
	}
}

void main(){
	parse_graph();

}
