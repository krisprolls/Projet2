#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <inttypes.h>
#include "mainHeader.h"
#include "producerHeader.h"


//constantes options
#define MAX_THREADS_TK "-maxthreads"
#define STDIN_TK "-stdin"
#define FILE_TK "file"
#define URL_TK  "http://"

extern const int STDIN_INPUT;
extern const int FILE_INPUT;
extern const int URL_INPUT;
extern sem_t available_thread; // quand un producteur termine son fichier
extern int available_thread_index;
extern sem_t available_switching; //main a traiter les switchs de producteurs 


int main(int argc, char **argv){	
	int inputsNumber = 0;
	int stdIn = 0;//stdIn pas utilise
	input* listOfInputs = NULL;// liste des entrees a traiter
	int maxThreads = 0;
	
	
	int  i;
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
			inputsNumber++;
			printf("Stdin set \n");
		}
		else if (strcmp(in, FILE_TK) == 0){ //fichier
			if(pushInput(&listOfInputs, argv[i+1]) != 0){
				//probleme de malloc, arrete le programme
				exit(errno);
			}
			else{
				printf("Added %s to inputs\n", argv[i+1]);
				i++;
				inputsNumber++;
			}
		}
		else if(strstr(in, URL_TK) != NULL){ //URL
			if(pushInput(&listOfInputs, argv[i]) != 0){
				//probleme de malloc, arrete le programme
				exit(errno);
			}
			else{
				printf("Added %s to inputs\n", argv[i]);
				inputsNumber++;
			}
		}
		else{ //erreur de format
			fprintf(stderr, "Erreur: mauvais format");
			exit(-1);
		}
	}
	
	printf("Number of inputs %d\n", inputsNumber);
	
	if(maxThreads == 0){ /// a voir
		maxThreads = 10;
		printf("maxthread was 0, assigned to %d\n", maxThreads);
	}
	
	//initialisation des threads et semaphores
	pthread_t* threads; //threads utilises
	threads = (pthread_t*) malloc(sizeof(pthread_t) * maxThreads);
	sem_init(&available_switching, 0, 1); //switching disponible 
	sem_init(&available_thread, 0, 0); // aucun thread n'a termine
	available_thread_index = -1;
	
	
	//lance tous les threads
	int err = launchAllThreads(maxThreads, &stdIn, &inputsNumber, &listOfInputs, &threads);
	if(err){
		fprintf(stderr, "Erreur dans launchAllThreads: %d\n", err);
		exit(err);
	}
	printf("All threads launched, inputs left: %d\n", inputsNumber);
	
	setbuf(stdout, NULL); // stoppe le buffer
	
	while(inputsNumber > 0){
		printf("Rentre ici \n");
		sem_wait(&available_thread); // attend qu'un thread ait termine
		//zone critique
		printf("Zone critique\n");
		err = pthread_join(threads[available_thread_index], NULL);
		if(err){
			fprintf(stderr, "Erreur dans pthread_join: %d\n", err);
			exit(err);
		}
		char* file = popInput(&listOfInputs);
		printf("filename: %s\n", file);
		if (file == NULL){
			fprintf(stderr, "Erreur dans popInput\n");
			exit(-1);
		}
		thread_arg t_arg;
		t_arg.file = file;
		t_arg.fileType = getFileType(file);
		t_arg.threadIndex = available_thread_index;
		printf("switching producer number: %d , now on %s \n", available_thread_index, file);
		err = pthread_create(&threads[available_thread_index], NULL, &threadLauncher, &t_arg);
		if(err){
			fprintf(stderr, "Erreur dans pthread_create: %d\n", errno);
			exit(errno);
		}
		available_thread_index = -1; // pas necessaire mais plus prudent
		inputsNumber--;
		sem_post(&available_switching); //permet aux thread de signaler 
		//qu'ils ont fini
	}	
	
	///TODO every producer to consumer
	//free(threads); //ne pas oublier !	
	exit(EXIT_SUCCESS);
}

/**
 * pushInput
 * Ajoute un fichier a la liste chainee d'entrees
 * @param: file, le fichier a ajouter
 * @param: list, la liste a modifier
 * @return: 0 si ok, un autre nombre sinon (contenu dans errno)
 **/ 
int pushInput(input** list, char* file){
	if(*list == NULL){
		*list = (input*) malloc(sizeof(input));
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
		input* tmp = *list;
		while(tmp -> next != NULL){
			tmp = tmp -> next;
		}
		tmp -> next = (input*) malloc (sizeof(input));
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
 * popInput
 * retourne la premiere entree contenue dans list
 * @param:list, la liste d'entree a modifier
 * @return: l'entree qui vient d'etre enlevee a la liste, NULL si liste vide
 **/ 
char* popInput(input** list){
	if (*list == NULL){
		fprintf(stderr, "Liste vide! \n");
		return NULL;
	}
	else{
		input* tmp = *list;
		*list = (*list) -> next;
		char* retVal = tmp -> file;
		free(tmp);
		tmp = NULL;
		return retVal;
	}
}



/**
 * launchAllThreads
 * Cette fonction lance maxthreads threads en alternant entre producteur
 * et consommteur
 * @param: maxThreads, nombre max de threads a lancer
 * @param: stdIn, pointeur vers un entier indiquant si l'entree standard
 * est utilisee
 * @param: inputsNumber, pointeur vers un entier indiquant le nombre
 * d'entree restant a traiter
 * @param: listOfInputs, pointeur vers une liste contenant les fichiers
 * d'entree
 * @param: threads, pointeur vers un tableau de threads
 * @return: 0 si pas d'erreur, valeur differente de 0 sinon
 **/ 
 int launchAllThreads(int maxThreads, int* stdIn, int* inputsNumber, input** listOfInputs, pthread_t** threads){
	const int ALT_PRODUCER = 1;
	const int ALT_CONSUMER = 0;
	int alternator = ALT_PRODUCER;
	//lance les threads alternativement (producteur, consommateur,
	// producteur...)
	
	int threadIndex;
	for(threadIndex = 0; threadIndex < maxThreads; threadIndex++){
	//lance tous les threads	
	
		if(alternator == ALT_PRODUCER && *inputsNumber > 0){
			//mode producteur
			
			thread_arg t_arg;
			if(*stdIn == 1){ 
				//on traite toujoours stdin en premier, cela permet
				//a l'utilisateur de ne pas attendre un thread disponible 
				//pour les entrees via la console
				t_arg.file = NULL;
				*stdIn = 0; //stdIn traite
				printf("stdIn bien lance\n");		
			}
			else{
				char* file = popInput(listOfInputs);
				if(file != NULL){					
					t_arg.file = file;
					printf("file bien lance\n");
				} 
				else {
					fprintf(stderr, "Erreur, plus de fichier a traiter\n");
					return(-1);
				}
			}
				
			t_arg.fileType = getFileType(t_arg.file);
			t_arg.threadIndex = threadIndex;	
					
			//lance le thread
			if(pthread_create(threads[threadIndex], NULL, &threadLauncher, &t_arg)){
				fprintf(stderr, "Error creating thread: %d\n", errno);
				return(errno);
			}
			(*inputsNumber)--;
			//passe en mode consommateur pour le prochain thread
			alternator = ALT_CONSUMER;
		}	
		else {
			//mode consommateur
			//lance le thread
			if(pthread_create(threads[threadIndex], NULL, &threadLauncher, NULL)){
				fprintf(stderr, "Error creating thread: %d\n", errno);
				return(errno);
			}
			//passe en mode producteur pour le prochain thread
			printf("consommateur bien lance\n");
			alternator = ALT_PRODUCER;
		}
	}/// fin du for qui lance tous les threads
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
 * getFileType
 * retourne le type de fichier
 * @param: file, le nom du fichier
 * @return: le nombre correspondant au type de fichier, voir les define
 **/ 
 int getFileType(char* file){
	 if(file == NULL){
		return STDIN_INPUT;
	}
	else if (strstr(file, URL_TK) != NULL)
		return URL_INPUT;
	else {
		return FILE_INPUT;
	}
 }


