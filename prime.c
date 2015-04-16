#include <stdlib.h>
#include <stdio.h>
#include <math.h>


static int global_length = 0;	// Variable globale qui servira dans la conversion de la liste en tableau d'entiers
								// Elle est incrementee plus bas dans prime_fact, remise a 0 dans list_to_array



// Structure qui represente une liste simplement chainee d'entiers. Elle servira de tableau dynamique.
typedef struct int_tab{
	int elem;
	struct int_tab * next;
}int_tab;



/**
 * Ajoute @new_int a la liste d'entiers @array
 * 
 * @return : 0 si ajouté, 1 si erreur (liste vide ou element a ajouter vide)
 */
int add(int new_int, int_tab *array)
{
	
	if(array==NULL){		//Liste vide
		return 1;
	}
	
	else if(array->elem==-1){ 	//Premier element a inserer dans array
		array->elem = new_int;
		array->next = NULL;
		return 0;
	}
	
	else{		//Liste non-vide
		
		int_tab *current = array;	// Element courant pour traverser la liste
		while((current->next != NULL)){
			current = current->next;
		}
		int_tab * element = malloc(sizeof(int_tab));
		element -> elem = new_int;
		element -> next = NULL;
		current -> next = element;
		return 0;
		printf("Element ajouté : %d\n", element->elem);	/// A enlever plus tard
	}
}




/**
 * Teste si un nombre est premier.
 * 
 * @number Le nombre qui est teste
 * @return : 1 si premier, 0 si le nombre est negatif
 *
 */
int is_prime(int number)
{
	if(number<0){
		printf("Le nombre est negatif, il est donc considere comme non-premier.\n");
		printf("\n");
		return 0;
	}
	else
	{
		int div = 2;
		while((double)div <= sqrt((double)number)){
			if(number % div == 0){
				return 0;
			}
			if(div % 2==0){
				div++;
			}
			else{
				div = div + 2;
			}
		}
	}
	return 1;
}


/**
 * Renvoie le nombre premier suivant celui en argument.
 * 
 * @number : le nombre duquel on part
 * @return : le nombre premier suivant @number
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
		while(is_prime(next) !=1){
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
int_tab * prime_fact(int number)
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
		int_tab * factors = malloc(sizeof(int_tab));	// La liste qui contiendra les facteurs premiers
		factors -> elem = -1;	// 1er element initialise a -1 pour montrer a la fonction "add" que la liste est vide
		int current_number = number;
		int div = 2;	// Diviseur courant
		int i;
		for(i=0;current_number != 1;i++){	// La boucle s'arrete lorsqu'on arrive a 1
			if(current_number % div ==0){	
				current_number = current_number / div;
				if(add(div, factors) == 1){		// Le nombre courant etait divisible par "div": on ajoute "div" comme facteur
					printf("Erreur lors de l'ajout d'un facteur\n");
					printf("\n");
					return NULL;
				}
				int_tab * curr = factors;
				int j;
				for(j=0;j<global_length+1;j++){		/// Simple verification de l'etat de la liste, je virerai ça plus tard
					printf("curr2.elem = %d\n", curr->elem);
					curr = curr->next;
				}
				printf("\n");
				global_length++;		// Incrementation de la variable globale
			}
			else{
				div = next_prime(div);		// current_number pas divisible par div, on prend le prochain nombre premier
			}
		}
		return factors;
		
	}
}
