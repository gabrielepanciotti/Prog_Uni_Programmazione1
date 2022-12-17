#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"

int main(){
	int scelta,termina=0;
	time_t t;
	srand((unsigned)time(&t));
	printf("===SABBIE===\n");
	do {
		printf("1)Crea cunicoli \n2)Cancella cunicoli \n3)Gioca \n4)Esci\n*Qualsiasi altro comando non è valido*\n");
		if(scanf("%d",&scelta)==0){
			printf("Hai inserito un carattere invece che un numero, gioco interroto,riavviare\n");
			termina=1;
		}
		else{
			switch (scelta) {
				case 1:
					crea_cunicoli();
					break;
				case 2:
					cancella_cunicoli();
					break;
				case 3:
					gioca();
					break;
				case 4:
					termina_gioco();
					termina=1;
					break;
				default:
					printf("Comando non valido\n");		
					break;
			}
		}
	} while(termina==0);
}
