#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "header.h"



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
