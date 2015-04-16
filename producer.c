#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <inttypes.h>
#include "producerHeader.h"

const int STDIN_ENTRY = 0;
const int FILE_ENTRY = 1;
const int SHAREDFILE_ENTRY = 2; 

sem_t available_thread; // quand un producteur termine son fichier
int available_thread_index;
sem_t available_switching; //main a traiter les switchs de producteurs 


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
		//readFile(arg->file);
	}
	else if(arg -> fileType == SHAREDFILE_ENTRY){
		//URL de fichier
	}
	else { 
		//stdin
		//printf("STDIN type in produce, filename: %s \n", arg->file);
		//readStdin();
	}
	//signale que le producteur est pret a etre reutilise
	sem_wait(&available_switching);
	available_thread_index = arg -> threadIndex;
	sem_post(&available_thread);
	
	return;
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
			//slotElem tmp = {filename, bEndian};
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
	//const char* SOURCE = "stdIn";
	const char* END = "Souhaitez-vous continuer le traitement via l'entree standard? Y/N \n";
	printf("Vous avez choisi d'utiliser l'entree standard, veuillez entrer les nombres que vous desirez traiter\n");
	printf("%s", END);
	int end = 0;
	while(end == 0){
		char e;
		scanf("%c", &e);
		if(e == 'Y' || e == 'y'){
			uint64_t bEndian;
			scanf("%" PRIu64",", &bEndian);
			bEndian = be64toh(bEndian);
			//slotElem tmp = {SOURCE, bEndian};
			///add to slots + mutex
			printf("Nombre traite!\n");
			printf("%s", END);
		}
		else{
			end++;
		}
	}
	printf("Fin du traitement.\n");
}
