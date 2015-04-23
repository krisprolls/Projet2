#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//#include <header.h>


// Structure qui aide a representer une liste simplement chainee d'entiers. Elle servira de tableau dynamique.
typedef struct factor{
	int elem;
	char * origin_file;
	struct factor * next;
}factor;


// Structure qui represente un facteur a inserer dans la liste finale. Elle contient le nombre d'occurrences du facteur en plus.
typedef struct global_factor{
	int elem;
	char * origin_file;
	struct global_factor * next;
	int occurr;
}global_factor;




/******************************************
***		Fonctions de factorisation		***
***		en facteurs premiers			***
*******************************************/


/**
 * Ajoute un facteur premier dans la liste de facteurs du nombre courant.
 * 
 * @new_int : facteur a ajouter
 * @filename : nom du fichier dans lequel se trouvait le nombre factorise
 * (si entree standard : filename = "stdin", si URL : filename = "<URL a lire>")
 * @return : 0 si ajouté, 1 si erreur (liste vide)
 * 
 */
int add(int new_int, char * filename, factor *array)
{
	
	if(array==NULL){		//Liste vide
		return 1;
	}
	
	else if(array->elem==-1){ 	//Premier element a inserer dans array
		array -> elem = new_int;
		array -> origin_file = filename;
		array -> next = NULL;
		return 0;
	}
	
	else{		//Liste non-vide
		
		factor *current = array;	// Element courant pour traverser la liste
		while((current->next != NULL)){
			current = current->next;
		}
		factor * element = malloc(sizeof(factor));	// Creation d'un element a inserer dans la liste
		element -> elem = new_int;		// L'element a inserer contient l'entier en argument
		element -> origin_file = filename;	// Ajout du nom de fichier d'origine dans lequel le nombre factorise se trouvait
		element -> next = NULL;
		current -> next = element;		//Insertion de l'element
		return 0;
}


/**
 * Teste si un nombre naturel est premier.
 * 
 * @pre : number > 1
 * @number : Le nombre qui est teste
 * @return : 1 si premier, 0 si pas premier, -1 si nombre negatif en argument
 *
 */
int is_prime(int number)
{
	if(number<0){
		printf("Le nombre est negatif.\n");
		printf("\n");
		return -1;
	}
	else if(number == 1 && number == 0){
		printf("Le nombre est egal a 0 ou 1, qui ne sont pas premiers.\n");
		printf("\n");
		return 0;
	}
	else
	{
		int div = 2;
		while((double)div <= sqrt((double)number)){	// On a seulement besoin de tester jusqu'a la racine carree de number 
			if(number % div == 0){
				return 0;
			}
			if(div % 2==0){
				div++;
			}
			else{
				div = div + 2;	// Permet d'eviter de restester tous les multiples de 2 comme diviseurs
			}
		}
	}
	return 1;	// Si aucun diviseur jusque racine(number) ne fonctionne, le nombre est premier.
}


/**
 * Renvoie le nombre premier suivant celui en argument, dans l'ordre croissant.
 * 
 * @pre : number > 0
 * @number : le nombre de depart
 * @return : le nombre premier suivant @number, ou @number si celui-ci est negatif.
 */
int next_prime(int number)
{
	if(number<0){
		printf("Le nombre est negatif. Retour de celui-ci.\n");
		printf("\n");
		return number;
	}
	else{
		int next=number+1;
		while(is_prime(next) !=1){	// Tant que next n'est pas premier, on l'incremente de 1.
			next++;
		}
		return next;
	}
}


/**
 * Decompose un nombre naturel en facteurs premiers.
 * 
 * @pre : number > 0
 * @number : Le nombre que l'on souhaite factoriser en facteurs premiers
 * @return : Un tableau d'entiers contenant les facteurs premiers
 * 
 */
factor * prime_fact(int number, char * filename)
{
	
	if(number <=0){
		printf("Le nombre a factoriser est inferieur a 0\n");
		printf("\n");
		return NULL;
	}
	
	else if(number == 1){
		printf("1 n'est pas factorisable en facteurs premiers\n");
		printf("\n");
		return NULL;
	}
	
	else{
		factor * factors = malloc(sizeof(factor));	// La liste qui contiendra les facteurs premiers
		factors -> elem = -1;	// 1er element initialise a -1 pour montrer a la fonction "add" que la liste est vide
		
		int current_number = number;	// Nombre courant
		int div = 2;	// Diviseur courant
		
		int i;
		for(i=0;current_number != 1;i++){	// La boucle s'arrete lorsqu'on arrive a 1
			if(current_number % div ==0){	
				current_number = current_number / div;
				if(add(div, factors, filename) == 1){		// Le nombre courant etait divisible par "div": on ajoute "div" comme facteur
					printf("Erreur lors de l'ajout d'un facteur\n");
					printf("\n");
					return NULL;
				}
				/*
				factor * curr = factors;
				int j;
				for(j=0;j<global_length+1;j++){		/// Simple verification de l'etat de la liste, je virerai ça plus tard
					printf("curr2.elem = %d\n", curr->elem);
					curr = curr->next;
				}
				printf("\n");
				*/
			}
			else{
				div = next_prime(div);	// current_number pas divisible par div, on prend le prochain nombre premier
			}
		}
		return factors;
		
	}
}



///////////////////////////////////////////////////////////////////////////////////////////




/**********************************
***		Fonctions inherentes	***
***		au consommateur			***
***********************************/



/**
 * Ajoute une liste de facteurs dans une liste de facteurs
 * globaux en suivant un tri par insertion selon l'occurrence
 * de chaque facteur global.
 * 
 * @list : liste de facteurs que l'on va inserer dans 'global'
 * @global : liste de global_factor dans laquelle est inseree 'list'
 * @return : 0 si ajouté, 1 si erreur (liste vide)
 * 
 */
int insert(global_factor ** global, int list_length, factor * list)
{
	
	if(global == NULL){
		return 1;
	}
	else if(list == NULL){
		return 1;
	}
	
	int i;
	for(i=0;i<list_length;i++){
		factor * current = list;	// Pointeur pour selectionner un element de 'list'
		int count = 0;	// compteur de la position jusqu'a l'indice i
		while(current -> next != NULL && count < i){	// Selection du facteur a l'indice i dans 'list'
			current = current -> next;
			count++;
		}
		
		global_factor * current_global = &global;	// Facteur traversant la liste globale
		int stop = 0;	// Boucle s'arrete si le facteur courant existe deja dans la liste globale
		while(current_global -> next != NULL && stop == 0){
			if(current_global -> elem == current -> elem){
				(current_global -> occurr)++;	// Incrementation de l'occurrence de ce facteur
				stop = 1;
			}
		}
		
		// Insertion en tete de liste avec occurrence de 1
		global_factor * new_fact = malloc(sizeof(global_factor));
		new_fact -> elem = current -> elem;
		new_fact -> occurr = 1;
		new_fact -> origin_file = current -> origin_file;
		new_fact -> next = global;
		global = new;
	}
	return 0;
}
		



/**
 * Fonction principale du consommateur.
 * La fonction lit les nombres dans le buffer partage,
 * les factorise et les place dans la liste chainee
 * de facteurs premiers, commune a tous les
 * consommateurs.
 * 
 * @pre : buffer != NULL, factor != NULL
 * @return : 0 si l'excecution s'est bien passee, 1 si erreur.
 * 
 */
int consumer(void)
{
	if(buffer == NULL){
		printf("Erreur : buffer vide. Rien a executer\n")
		printf("\n");
		return 1;
	}
	
	if(global_list == NULL){	// Liste de facteurs partagee entre les consommateurs, consideree comme variable globale
		return 0;
	}
	
	factor * fac_list = malloc(sizeof(factor));	// Liste intermediaire de facteurs dans laquelle on stock la factorisation des nombres du buffer;
												// Cette liste sera ensuite ajoutee a la liste chainee globale.
	
	
	
	sem_wait(&full);	// Attente qu'au moins un slot du buffer partage est disponible
	
	if(pthread_mutex_lock(mutex_buff)!=0){
		printf("Erreur mutex_lock\n");
		return 1;
	}
	
	int i;		// Parcours du buffer puis factorisation de chaque nombre parcouru
	for(i=0;i<buffer_size;i++){
		if((buffer[i] -> treated) == 0){		/// Check structure
			fac_list = prime_fact(buffer[i] -> number, buffer[i] ->  origin_file);	// Factorisation
		}
	}
	
	if(pthread_mutex_unlock(mutex_buff)!=0){
		printf("Erreur mutex_unlock\n");
		return 1;
	}
	
	sem_post(&empty);	// Un slot du buffer a ete consomme et est libere
	
	
	// Insertion de la liste de facteurs intermediaire dans la liste globale
	if(pthread_mutex_lock(mutex_conso)!=0){
		printf("Erreur mutex_lock\n");
		return 1;
	}
	
	if(insert(&global_list, fac_list) == 1){
		printf("Erreur lors de l'insertion des facteurs dans la liste globale\n");
		printf("\n");
	}
	
	if(pthread_mutex_unlock(mutex_conso)!=0){
		printf("Erreur mutex_unlock\n");
		return 1;
	}

}



int main(int argc, char* argv[])
{
	return EXIT_SUCCESS;
}
