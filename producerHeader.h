
//element du buffer producer-consumer
typedef struct slotElem{
	char* file;
	uint64_t number;
} slotElem;

//argument a passer au lancement du thread
typedef struct thread_arg{
	char* file;
	int fileType;
	int threadIndex;
} thread_arg;

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
