#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int iterar_directorios(int fd, char* filtro,int sensitive, char* path);

int main (int argc, char *argv[]){
	
	int case_sensitive = 0;
	char* filtro;
	if (argc < 2 || argc > 3){	// Cantidad de argumentos acorde
		fprintf(stderr, "Usage %s [-i] filename\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	else if (argc == 3)		// Si se utilizo opción sensitive
	{		
		if ((strstr(argv[1],"-i") == NULL)||(strstr("-i",argv[1]) == NULL)){	// Validación argumento
			fprintf(stderr, "Usage %s [-i] filename\n",argv[0]);
			exit(EXIT_FAILURE);
		}
		else{		
			case_sensitive = 1;
			filtro = argv[2];
		}	
	}
	else
		filtro = argv[1];
	
	DIR * directorio_inicial = opendir(".");
	if (directorio_inicial == NULL){
		fprintf(stderr,"Error al abrir directorio inicial");
		exit(EXIT_FAILURE);
	}
	
	char path[256];
	for (int i=0;i<256;i++)
		path[i] = '\0';

	iterar_directorios(dirfd(directorio_inicial), filtro, case_sensitive, path);

	return 0;
}

int iterar_directorios(int fd, char* filtro,int sensitive, char* path){
	DIR * directorio = fdopendir(fd);
	if (directorio == NULL){
                fprintf(stderr,"Error al abrir subdirectorio");
                exit(EXIT_FAILURE);
        }
	
	struct dirent *file;
	int primer_iteracion = 1;
	
	while (primer_iteracion == 1 || file != NULL){
		 if (primer_iteracion == 1)
			 primer_iteracion = 0;

		 file = readdir(directorio);
		 if (file != NULL){
		 	if ((file->d_name[0] != '.' && file->d_name[1] != '\0')||(file->d_name[0] != '.' && file->d_name[1] && file->d_name[2] != '\0')){
				if (file->d_type == 4){
					char path_2[256];
					strcpy(path_2,path);
					
					int largo = 0;			// Detecta hasta donde fue escrito el path
					while (path_2[largo] != '\0')
						largo++;
					path_2[largo] = '/';

					for (int i = 0; i < strlen(file->d_name); i++)	// Agrega el nuevo directorio
						path_2[largo + 1 + i] = file->d_name[i];

					int nuevo_directorio_fd = openat(dirfd(directorio), file->d_name, O_DIRECTORY);
					iterar_directorios(nuevo_directorio_fd, filtro, sensitive, path_2);
				}
				else {
					int print = 0;
					if ((sensitive == 1 && strcasecmp(file->d_name,filtro)) == 0)
						print = 1;
					else if ((sensitive == 0 && strcmp(file->d_name,filtro)) == 0)
						print = 1;
						
					if (print == 1){
						int i = 0;
						while (path[i] != '\0'){
							printf("%c",path[i]);
							i++;
						}
						printf("/%s\n",file->d_name);	
					}
				}
		 	}
		 }
	}
	
	return 0;
}

