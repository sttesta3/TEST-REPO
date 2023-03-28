#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int main(int argc, char *argv[])
{
    printf("Hola, soy PID %d:\n",getpid());
    int pipefd1[2], pipefd2[2];

    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
	fprintf(stderr,"Error pipe\n");
	exit(EXIT_FAILURE);
    }
    
    printf("  - primer pipe me devuelve: [%d, %d]\n",pipefd1[0],pipefd1[1]);
    printf("  - segundo pipe me devuelve: [%d, %d]\n",pipefd2[0],pipefd2[1]);

    pid_t PID;
    int buf1;
    int buf2;

    PID = fork();
    if (PID == -1) {
	fprintf(stderr,"Error fork\n");
        exit(EXIT_FAILURE);
    }

    if (PID == 0) {	    			/* Hijo */
        close(pipefd1[1]);          		/* Cerrar escritura pipe 1 */
	close(pipefd2[0]);			/* Cerrar lectura pipe 2 */
	
        while (read(pipefd1[0], &buf1, 4) > 0)	/* Espera hasta que leas algo del pipe */
	close(pipefd1[0]);
	
	printf("Donde fork me devuelve %d:\n",PID);
	printf("  - getpid me devuelve: %d\n",getpid());
	printf("  - getppid me devuelve: %d\n",getppid());
	printf("  - recibo valor %d vía fd=%d\n",buf1,pipefd1[0]);
	printf("  - envío valor %d a través de fd=%d\n",buf1,pipefd2[1]);
	
	write(pipefd2[1], &buf1, 4);
	close(pipefd2[1]);

	_exit(EXIT_SUCCESS);
    } 
    else {            				/* Padre */
        close(pipefd1[0]);          		/* Cerrar lectura pipe 1 */
        close(pipefd2[1]);	    		/* Cerrar escritura pipe 2 */

	srand(time(NULL));			/* Numero Random */
	int num_random = rand();
	
	printf("Donde fork me devuelve %d:\n",PID);
	printf("  - getpid me devuelve: %d\n",getpid());
	printf("  - getppid me devuelve: %d\n",getppid());
	printf("  - random me devuelve: %d\n",num_random);
	printf("  - envío valor %d a través de fd=%d\n",num_random,pipefd1[1]);
	
	write(pipefd1[1], &num_random, 4);	/* Escribir valor al hijo*/
        close(pipefd1[1]);          		
        
	while (read(pipefd2[0], &buf2, 4) > 0)	/* Espera hasta leer del hijo */	
	close(pipefd2[0]);	
	wait(NULL);                	

	printf("Hola, de nuevo PID %d:\n",getpid());
	printf("  - recibí valor %d vía fd=%d\n",buf2,pipefd2[0]);

        exit(EXIT_SUCCESS);
    }
}
