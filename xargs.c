#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef NARGS
#define NARGS 4
#endif

int ejecutar_comando(char* comando, char* args[]);

int main (int argc, char *argv[]){
	
	if (argc != 2){
		fprintf(stderr,"Usage: %s <command>\n",argv[0]);
		return 1;
	}
	int n = NARGS;
	char* comando = argv[1];
	char *linea = NULL;
	size_t buffer = 0;

	int i = 1;
	char* argumentos[n+1];
	argumentos[0] = comando;
	for (int j = 1; j <= n+1; j++)
		argumentos[j] = NULL;
	
	while(getline(&linea ,&buffer, stdin) != -1){
		argumentos[i] = strdup(linea);
		argumentos[i][strlen(argumentos[i]) - 1] = '\0';
		i++;
			
		if (i == n + 1){
			ejecutar_comando(comando,argumentos);
			i = 1;
		}
	}
	
	if (argumentos[1] != NULL)
		ejecutar_comando(comando,argumentos);

	free(linea);
	return 0;
}

int ejecutar_comando(char* comando, char* args[]){

	int PID = fork();
	if (PID < 0){
		fprintf(stderr,"Error fork\n");
		return 1;
	}
	else if (PID == 0){
		// Hijo ejecuta el comando
		execvp(comando,args);
		exit(EXIT_SUCCESS);
	}
	else{
		wait(NULL);
		// Liberar argumentos
		int i = 1;
		while (args[i] != NULL){
			free(args[i]);
			args[i] = NULL;
			i++;
		}
		
		// EXIT PADRE
		return 0;
	}
}
