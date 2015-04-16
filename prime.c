#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
