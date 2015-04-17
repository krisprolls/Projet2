/*
 * FICHIER POUR LA DECLARATION DES FONCTIONS
 * 
 **/
//represente un fichier d'entree
typedef struct input{
	char* file;
	struct input* next;
}input;


/*
 * Liste chainee d'entrees, methodes de gestion
 */
/**
 * pushInput
 * Ajoute un fichier a la liste chainee d'entrees
 * @param: file, le fichier a ajouter
 * @param: list, la liste a modifier
 * @return: 0 si ok, un autre nombre sinon (contenu dans errno)
 **/ 
extern int pushInput(input** list, char* file);

/**
 * popInput
 * retourne la premiere entree contenue dans list
 * @param:list, la liste d'entree a modifier
 * @return: l'entree qui vient d'etre enlevee a la liste, NULL si liste vide
 **/ 
extern char* popInput(input** list);


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
 * @param: inputsNumber, pointeur vers un entier indiquant le nombre
 * d'entree restant a traiter
 * @param: listOfInputs, pointeur vers une liste contenant les fichiers
 * d'entree
 * @param: threads, pointeur vers un tableau de threads
 * @return: 0 si pas d'erreur, valeur differente de 0 sinon
 **/ 
extern int launchAllThreads(int maxThreads, int* stdIn, int* inputsNumber, input** listOfInputs, pthread_t** threads);

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

