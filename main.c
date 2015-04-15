#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <errnoh>
#include <endian.h>
#include <inttypes.h>
#include "mainHeader.h"

#define STDIN_ENTRY 0
#define FILE_ENTRY 1
#define SHAREDFILE_ENTRY 2 



int main(int argc, char **argv)
{
	
	return 0;
}

/**
 * threadLauncher
 * Lance les threads en producteur ou consommateur en fonction de
 * l'argument passe.
 * @param: arg, est soit NULL, soit une structure thread_arg contenant
 * le fichier a lire, le type de fichier et l'indice du thread
 * @return: NULL quand le thread a termine
 **/ 
void* threadLauncher(void* arg){
	if (arg == NULL){ //consumer
		///TODO consume
	}
	else {//producer
		thread_arg* t_arg = (thread_arg*) arg;
		produce(t_arg);
	}
	pthread_exit(NULL);
}

/**
 * produce
 * Lit le fichier passe en argument et convertit les nombres en 
 * representation locale
 * @param: arg, la structure contenant, le fichier a lire, le type de 
 * fichier et l'indice du thread
 **/ 
void produce(thread_arg* arg){
	if(arg -> fileType == FILE_ENTRY){
		//fichier simple
		readFile(arg->file);
	}
	else if(arg -> fileType == SHAREDFILE_ENTRY){
		//URL de fichier
	}
	else { 
		//stdin
		readStdin();
	}
	///TODO semaphore
}

/**
 * readFile
 * Lit le fichier passe en argument, convertit les nombres en 
 * representation locale et les place dans un slot pour etre factorises
 * @param: filename, le nom du fichier a lire
 **/ 
void readFile(char* filename){
	///voir format des fichiers
	FILE* file = NULL;
	file = fopen((const char*) filename, "r"); //read-mode
	if(file != NULL){
		while(feof(file) == 0){
			uint64_t bEndian;
			fscanf(file, "%" PRIu64",", &bEndian);
			bEndian = be64toh(bEndian);
			slotElem tmp = {filename, bEndian};
			///add to slots + mutex
			fclose(file);
		}
	}
}

/**
 * readStdin
 * Lit les nombres passes via l'entree standard, les convertit 
 * representation locale et les place dans un slot pour etre factorises
 **/ 
void readStdin(){
	const char* STDIN = "stdIn";
	const char* END = "Souhaitez-vous terminer le traitement via l'entree standard? Y/N \n";
	printf("Vous avez choisi d'utiliser l'entree standard, veuillez entrer les nombres que vous desirez traiter\n");
	printf(END);
	int end = 0;
	while(end == 0){
		char e;
		scanf("%c", &e);
		if(e == 'Y' || e == 'y'){
			uint64_t bEndian;
			scanf("%" PRIu64",", &bEndian);
			bEndian = be64toh(bEndian);
			slotElem tmp = {filename, bEndian};
			///add to slots + mutex
			printf("Nombre traite!\n");
			printf(END);
		}
		else{
			end++;
		}
	}
	printf("Fin du traitement.\n");
}


