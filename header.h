/*
 * FICHIER POUR LA DECLARATION DES FONCTIONS
 * 
 **/
 
 //Sert a representer une entree (liste chainee car on en connait pas
//le nombre d'entrees totales
typedef struct entry{
	char* file;
	 struct entry* next;
}entry;
/**
 * pushEntry
 * Ajoute un fichier a la liste chainee d'entrees
 * @param: file, le fichier a ajouter
 * @param: list, la liste a modifier
 * @return: 0 si ok, un autre nombre sinon (contenu dans errno)
 **/ 
int pushEntry(entry** list, char* file);
