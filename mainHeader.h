/*
 * FICHIER POUR LA DECLARATION DES FONCTIONS
 * 
 **/
//argument a passer au lancement du thread
typedef struct thread_arg{
	char* file;
	int fileType;
	int threadIndex;
} thread_arg;

//element du buffer producer-consumer
typedef struct slotElem{
	char* file;
	uint64_t number;
} slotElem;

//represente un fichier d'entree
typedef struct entry{
	char* file;
	struct entry* next;
}entry;


/*
 * Liste chainee d'entrees, methodes de gestion
 */
/**
 * pushEntry
 * Ajoute un fichier a la liste chainee d'entrees
 * @param: file, le fichier a ajouter
 * @param: list, la liste a modifier
 * @return: 0 si ok, un autre nombre sinon (contenu dans errno)
 **/ 
extern int pushEntry(entry** list, char* file);

/**
 * popEntry
 * retourne la premiere entree contenue dans list
 * @param:list, la liste d'entree a modifier
 * @return: l'entree qui vient d'etre enlevee a la liste, NULL si liste vide
 **/ 
extern char* popEntry(entry** list);


/*
 * Methodes pour lancer les threads (mainHelpers)
 */ 

/**
 * launchAllThreads
 * Cette fonction lance maxthreads threads en alternant entre producteur
 * et consommteur
 * @param: maxThreads, nombre max de threads a lancer
 * @param: stdIn, pointeur vers un entier indiquant si l'entree standard
 * est utilisee
 * @param: entriesNumber, pointeur vers un entier indiquant le nombre
 * d'entree restant a traiter
 * @param: listOfEntries, pointeur vers une liste contenant les fichiers
 * d'entree
 * @param: threads, pointeur vers un tableau de threads
 * @return: 0 si pas d'erreur, valeur differente de 0 sinon
 **/ 
extern int launchAllThreads(int maxThreads, int* stdIn, int* entriesNumber, entry** listOfEntries, pthread_t** threads);

/**
 * threadLauncher
 * Lance les threads en producteur ou consommateur en fonction de
 * l'argument passe.
 * @param: arg, est soit NULL, soit une structure thread_arg contenant
 * le fichier a lire, le type de fichier et l'indice du thread
 * @return: NULL quand le thread a termine
 **/ 
extern void* threadLauncher(void* arg);

/**
 * getFileType
 * retourne le type de fichier
 * @param: file, le nom du fichier
 * @return: le nombre correspondant au type de fichier, voir les define
 **/ 
extern int getFileType(char* file);


/*
 * Methodes reservees au producteur
 */ 
/**
 * produce
 * Appelle la methode adaptee au type de fichier pour lire le fichier
 * signale que le thread a termine le fichier apres avoir lu l'entierete
 * du fichier
 * @param: arg, la structure contenant, le fichier a lire, le type de 
 * fichier et l'indice du thread
 **/ 
extern void produce(thread_arg* arg);

/**
 * readFile
 * Lit le fichier passe en argument, convertit les nombres en 
 * representation locale et les place dans un slot pour etre factorises
 * @param: file, le nom du fichier a lire
 **/ 
extern void readFile(char* file);

/**
 * readStdin
 * Lit les nombres passes via l'entree standard, les convertit 
 * representation locale et les place dans un slot pour etre factorises
 **/ 
extern void readStdin();
