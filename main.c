#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <inttypes.h>
#include "mainHeader.h"

//define pour main
#define ALT_PRODUCER 0
#define ALT_CONSUMER 1

#define STDIN_ENTRY 0
#define FILE_ENTRY 1
#define SHAREDFILE_ENTRY 2 


int main(int argc, char **argv)
{
	int stdIn = 0; //stdIn pas utilise
	entry* listOfEntries = NULL;
	int maxthreads, i;
	int entriesNumber = 0;
	
	//constante options
	const char* maxThreadsTk = "-maxthreads";
	const char* stdInTk = "-stdin";
	const char* fileTk = "file";
	const char* urlTk = "http://";
	
	//Parcoure tous les arguments de lancement du programme
	for(i = 1; i < argc; i++){
		char* in = argv[i];
		if(strcmp(in, maxThreadsTk) == 0){ //maxthreads
			maxthreads = atoi(argv[i+1]);
			i++;
			//on passe 2 arguments puisqu'on a deja utilise le nombre
			// de threads max
			printf("Maxthreads set to %d\n",maxthreads);
		}
		else if (strcmp(in, stdInTk) == 0){ //stdIn
			stdIn = 1; // stdIn utilise
			entriesNumber++;
			printf("Stdin set \n");
		}
		else if (strcmp(in, fileTk) == 0){ //fichier
			if(pushEntry(&listOfEntries, argv[i+1]) != 0){
				//probleme de malloc, arrete le programme
				exit(errno);
			}
			else{
				printf("Added %s to entries\n", argv[i+1]);
				i++;
				entriesNumber++;
			}
		}
		else if(strstr(in, urlTk) != NULL){ //URL
			if(pushEntry(&listOfEntries, argv[i]) != 0){
				//probleme de malloc, arrete le programme
				exit(errno);
			}
			else{
				printf("Added %s to entries\n", argv[i]);
				entriesNumber++;
			}
		}
		else{ //erreur de format
			fprintf(stderr, "Erreur: mauvais format");
			exit(-1);
		}
	}
	
	printf("Number of entries %d\n", entriesNumber);
	
	int alternator = ALT_PRODUCER;
	//lance les threads alternativement (producteur, consommateur,
	// producteur...)
	while(entriesNumber > 0){
			///TODO complete the thread launching phase here
	}
	
	return 0;
}

/**
 * pushEntry
 * Ajoute un fichier a la liste chainee d'entrees
 * @param: file, le fichier a ajouter
 * @param: list, la liste a modifier
 * @return: 0 si ok, un autre nombre sinon (contenu dans errno)
 **/ 
int pushEntry(entry** list, char* file){
	if(*list == NULL){
		*list = (entry*) malloc(sizeof(entry));
		if (*list != NULL){
			(*list) -> file = file;
			(*list) -> next = NULL;
		}
		else {
			fprintf(stderr, "Error: malloc failed. %d\n", errno);
			return errno;
		}
	}
	else {
		entry* tmp = *list;
		while(tmp -> next != NULL){
			tmp = tmp -> next;
		}
		tmp -> next = (entry*) malloc (sizeof(entry));
		if ((tmp -> next) != NULL){
			(tmp -> next) -> file = file;
			(tmp -> next) -> next = NULL;
		}
		else {
			fprintf(stderr, "Error: malloc failed. %d\n", errno);
			return errno;
		}
			
	}
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
