#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int filtrar (int pipefd[2]);
int string_a_int(char* str);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <int>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	int N = string_a_int(argv[1]);

	/* Enviar valores */
	int pipefd[2];
	if (pipe(pipefd) == -1){
		fprintf(stderr,"Error pipe inicial\n");
		exit(EXIT_FAILURE);
	}
	else{
		for (int i=2; i <= N; i++)
			write(pipefd[1], &i, 4);
	}
	close(pipefd[1]);

	filtrar(pipefd);
}


int filtrar (int pipefd[2]){
	int PID = fork();

	if (PID == -1){
		fprintf(stderr,"Error fork\n");
		exit(EXIT_FAILURE);
	}
	else if (PID != 0){
		wait(NULL);
		exit(EXIT_SUCCESS);
	}
	else {
		int buf;
		int primer_valor = 1;
		int filtro;

		int pipefd_cd[2];
		if (pipe(pipefd_cd) == -1){
			fprintf(stderr,"Error pipe hijo\n");
			exit(EXIT_FAILURE);
		}
		
		while (read(pipefd[0], &buf, 4) > 0){
			if (primer_valor == 1){
				filtro = buf;
				primer_valor = 0;
				printf("primo %d\n",filtro);
			}
			else{
				if (buf%filtro != 0)
					write(pipefd_cd[1], &buf, 4);
			}
		}

		// printf("primer valor: %d\n",primer_valor);
		close(pipefd[0]);
		close(pipefd_cd[1]);

		if (primer_valor != 1)
			filtrar(pipefd_cd);
		else
			close(pipefd_cd[0]);
		exit(EXIT_SUCCESS);
	}
}

int string_a_int(char* str){
	int largo = 0;
        while (*(str + largo) != '\0')
                largo++;
	
	int resultado;
        int temp;
	int base;
        for (int i = 0; i < largo; i++){
		base = 1;
                temp = *(str + i);
                if (temp < 48 || temp > 57){
                        fprintf(stderr, "Valor invalido\n");
                        exit(EXIT_FAILURE);
                }
                else {
                        for (int ii = 0; ii < (largo - 1 - i); ii++)
				base *= 10;
			resultado += (temp % 48)*base;
		}
        }

	return resultado;
}
