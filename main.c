#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <inttypes.h>
#include "mainHeader.h"

//constantes options
#define MAX_THREADS_TK "-maxthreads"
#define STDIN_TK "-stdin"
#define FILE_TK "file"
#define URL_TK  "http://"

#define STDIN_ENTRY 0
#define FILE_ENTRY 1
#define SHAREDFILE_ENTRY 2 




int main(int argc, char **argv)
{
	int stdIn = 0; //stdIn pas utilise
	entry* listOfEntries = NULL;
	int maxThreads, i;
	int entriesNumber = 0;
	
	
	//Parcoure tous les arguments de lancement du programme
	for(i = 1; i < argc; i++){
		char* in = argv[i];
		if(strcmp(in, MAX_THREADS_TK) == 0){ //maxthreads
			maxThreads = atoi(argv[i+1]);
			i++;
			//on passe 2 arguments puisqu'on a deja utilise le nombre
			// de threads max
			printf("Maxthreads set to %d\n",maxThreads);
		}
		else if (strcmp(in, STDIN_TK) == 0){ //stdIn
			stdIn = 1; // stdIn utilise
			entriesNumber++;
			printf("Stdin set \n");
		}
		else if (strcmp(in, FILE_TK) == 0){ //fichier
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
		else if(strstr(in, URL_TK) != NULL){ //URL
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
	
	
	
	if(maxThreads == 0){ /// a voir
		maxThreads = 10;
		printf("maxthread was 0, assigned to %d\n", maxThreads);
	}
	
	const int ALT_PRODUCER = 1;
	const int ALT_CONSUMER = 0;
	int alternator = ALT_PRODUCER;
	//lance les threads alternativement (producteur, consommateur,
	// producteur...)
	pthread_t threads[maxThreads];
	int threadIndex;
	
	for(threadIndex = 0; threadIndex < maxThreads; threadIndex++){
	//lance tous les threads	
	
		if(alternator == ALT_PRODUCER && entriesNumber > 0){
			//mode producteur
			
			thread_arg t_arg;
			if(stdIn == 1){ 
				//on traite toujoours stdin en premier, cela permet
				//a l'utilisateur de ne pas attendre un thread disponible 
				//pour les entrees via la console
				t_arg.file = NULL;
				t_arg.fileType =  STDIN_ENTRY;
				t_arg.threadIndex =  threadIndex;
				stdIn = 0;
				entriesNumber--;
				printf("stdIn bien lance\n");		
			}
			
			else{
				char* file = popEntry(&listOfEntries);
				
				if(file != NULL){					
					if(strstr(file, URL_TK) != NULL){ //fichier URL
						t_arg.file = NULL;
						t_arg.fileType =  SHAREDFILE_ENTRY;
						t_arg.threadIndex = threadIndex;
					}
					else {//fichier normal
						t_arg.file = NULL;
						t_arg.fileType = FILE_ENTRY;
						t_arg.threadIndex = threadIndex;
					}
					//lance le thread
					if(pthread_create(&threads[threadIndex], NULL, &threadLauncher, &t_arg)){
						fprintf(stderr, "Error creating thread: %d\n", errno);
						exit(errno);
					}	
					entriesNumber--;
					printf("file bien lance\n");
				} 
				
				else { //erreur de popEntry
					fprintf(stderr, "Erreur, plus de fichier a traiter\n");
				}
			}	
			
			//passe en mode consommateur pour le prochain thread
			
			alternator = ALT_CONSUMER;
		}	
		else {
			//mode consommateur
			//lance le thread
			if(pthread_create(&threads[threadIndex], NULL, &threadLauncher, NULL)){
				fprintf(stderr, "Error creating thread: %d\n", errno);
				exit(errno);
			}
			//passe en mode producteur pour le prochain thread
			printf("consommateur bien lance\n");
			alternator = ALT_PRODUCER;
		}
	}// fin du for qui lance tous les threads
	
	
	
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
 * popEntry
 * retourne la premiere entree contenue dans list
 * @param:list, la liste d'entree a modifier
 * @return: l'entree qui vient d'etre enlevee a la liste, NULL si liste vide
 **/ 
char* popEntry(entry** list){
	if (*list == NULL){
		fprintf(stderr, "Liste vide! \n");
		return NULL;
	}
	else{
		entry* tmp = *list;
		*list = (*list) -> next;
		char* retVal = tmp -> file;
		free(tmp);
		tmp = NULL;
		return retVal;
	}
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
	printf("%s", END);
	int end = 0;
	while(end == 0){
		char e;
		scanf("%c", &e);
		if(e == 'Y' || e == 'y'){
			uint64_t bEndian;
			scanf("%" PRIu64",", &bEndian);
			bEndian = be64toh(bEndian);
			slotElem tmp = {"stdIn", bEndian};
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
