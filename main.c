#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

int main(){
	int d;
	time_t t;
	srand((unsigned)time(&t));
	insI();

	printf("***Benvenuto in SABBIE***\n");

	do {
		printf("\n1)Crea cunicoli 2)Gioca 3)Esci\n*Qualsiasi altro tasto ti fara' uscire*\n");
		scanf("%d",&d);
		clean_stdin();
		switch (d) {
			case 1:
				crea_cunicoli();
				break;
			case 2:
				gioca();
				break;
			case 3:
				uscita_programma();
				break;
			default:
			uscita_programma();
			break;
		}
	} while(1);
}
