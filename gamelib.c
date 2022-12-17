#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gamelib.h"
/*
 * Ogni giocatore è assegnato ad una posizione degli array seguenti
 * L'array partenza contiene le caverne da cui partono i giocatori
 * L'array ultima contiene le ultime caverne create dai giocatori
 * L'array scavatrice contiene le scavatrice dei giocatori
 * */
#define num_giocatori 3  //Stabilisci il numero di giocatori che giocano
#define num_caverne_minime 3  //Stabilisci il numero di caverne minime da creare prima di chiudere il cunicolo
static Caverna * partenza[num_giocatori];
static Caverna * ultima[num_giocatori];

static Scavatrice * scavatrice[num_giocatori];

static int turno; //Indica di quale dei due giocatori è il turno, se turno 0 è il turno di arvais se 1 è turno di hartonen
static int mappa_terminata[num_giocatori];	//mappa_terminata[0] per arvais,mappa_terminata[1] per hartonen
static int mappe_terminate; //flag per indicare quando tutte le mappe dei giocatori sono state terminate e si può procedere alla fase successiva
static int num_caverne[num_giocatori];	//Numero di caverne create per giocatore
static int ostacolo[num_giocatori]; //Indica se è presente un ostacolo nella grotta di ciascun giocatore
static int vittoria[num_giocatori]; //Settato inizialmente a 0 diventa a 1 l'elemento in posizione del giocatore corrispondente quando vince
static int sconfitto[num_giocatori]; //Settato inizialmente a 0 diventa a 1 l'elemento in posizione del giocatore corrispondente quando perde
static int uscito[num_giocatori]; //Settato inizialmente a 0 diventa a 1 l'elemento in posizione del giocatore corrispondente quando è uscito dalla grotta
static int prob_scontro; //Probabilità che avvenga uno scontro

static void inizializzazione(); //inizializza le variabili
static void ins_caverna(); //Inserisce una caverna
static void canc_caverna(); //Cancella una caverna
static void stampa_cunicolo();
static void chiudi_cunicolo();
static int casuale(int,int); //Casuale tra 3 possibilità
static int casuale2(int,int,int); //Casuale tra 4 possibilità
static int casuale3(int); //Casuale tra 2 possibilità
static void avanza(); 
static void abbatti();
static void aggira();
static void esci();
static void effetto_avanzamento(); //Stato,imprevisto e melasse della nuova caverba
static void sposta_melassa(); //Sposta la melassa da un serbatoio all'altro
static void scontro(); //Scontro tra due scavatrici
static void controllo_vittoria(); //Controlla se il giocatore che ha il turno è l'ultimo ad essere rimasto in vita
static int controllo_unico();  //Controlla se il giocatore che ha il turno è l'ultimo rimasto in vita dentro la grotta
static int random_num(int); //Restituisce un numero random con massimo dato come parametro
static int enum_stato(int); //Ritorna l'enum in base al valore in input
static int enum_melassa(int);
static int enum_imprevisto(int);

/*
 * SEZIONE CREA_CUNICOLI()
 * */

extern void crea_cunicoli(){ //Preparazione mappa
	int scelta;
	if(mappe_terminate==0){
		inizializzazione(); //Inizializza gli array dei giocatori
	}
	else{
		mappe_terminate=0;
		for(int i=0;i<num_giocatori;i++){
			mappa_terminata[i]=0;
		}
	}
	turno=0;
	printf("===FASE CREAZIONE MAPPE AVVIATA===\n");
	do {
		printf("===GIOCATORE N. %d===\n",turno);
		printf("Scegli tra una delle seguenti opzioni:\n1)Inserisci caverna \n2)Cancella caverna \n3)Stampa cunicolo \n4)Chiudi cunicoli\n*Qualsiasi altro comando non è valido*\n");
		printf("Caverne attuali create:%d\n",num_caverne[turno]);
		if(scanf("%d",&scelta)==0){
			printf("Hai inserito un carattere invece che un numero, creazione interrota,riavviare\n");
			break;
		}
		else{
			switch (scelta) {
				case 1:
					printf("===FASE INSERISCI CAVERNA===\n");
					ins_caverna();
					break;
				case 2:
					printf("===FASE CANCELLA CAVERNA===\n");
					canc_caverna();
					break;
				case 3:
					printf("===FASE STAMPA CUNICOLO===\n");
					stampa_cunicolo();
					break;
				case 4:
					printf("===FASE CHIUDI CUNICOLI===\n");
					if(num_caverne[turno]<num_caverne_minime){
						printf("Non hai inserito abbastanza caverne(min %d)\n",num_caverne_minime);
						break;
					}
					chiudi_cunicolo();
					turno++;
					break;
				default:
					printf("Comando non valido\n");		
					break;
			}
		}
	} while(mappe_terminate==0);
	printf("===FASE CREAZIONE MAPPE TERMINATA===\n");
}

static void inizializzazione(){
	for(int x=0;x<num_giocatori;x++){
		partenza[x]=(Caverna *) malloc(sizeof(Caverna));
		partenza[x]->avanti=NULL;
		partenza[x]->destra=NULL;
		partenza[x]->sinistra=NULL;
		partenza[x]->indietro=NULL;
		partenza[x]->imprevisto=enum_imprevisto(0);
		partenza[x]->melassa=enum_melassa(0);
		partenza[x]->stato=enum_stato(0);
		ultima[x]=partenza[x];
		scavatrice[x]=(Scavatrice *) malloc(sizeof(Scavatrice));
		scavatrice[x]->melassa_energia=10;
		scavatrice[x]->melassa_serbatoio=0;
		scavatrice[x]->pos=ultima[x];
		mappa_terminata[x]=0;
		ostacolo[x]=0;
		vittoria[x]=0;
		sconfitto[x]=0;
		uscito[x]=0;
		num_caverne[x]=0;
	}
	mappe_terminate=0;
}	

static void ins_caverna(){
	int stato=0,direzione=0;
	Caverna *cav=(Caverna *) malloc(sizeof(Caverna));;
	if(ultima[turno]->indietro==NULL){ //Se è la prima caverna ad essere creata, creo solo una caverna in avanti
		ultima[turno]->avanti=(Caverna *) malloc(sizeof(Caverna));
		cav=ultima[turno];
		ultima[turno]=ultima[turno]->avanti;
		ultima[turno]->indietro=cav;
		printf("Hai creato la prima caverna\n");
	}
	else{
		printf("Dove vuoi inserire la prossima caverna? (1=avanti,2=destra,3=sinistra)\n");
		scanf("%d",&direzione);
		switch(direzione){
			case 1:
				ultima[turno]->avanti=(Caverna *) malloc(sizeof(Caverna));
				cav=ultima[turno];
				ultima[turno]=ultima[turno]->avanti;
				ultima[turno]->indietro=cav;
				printf("Hai creato la caverna avanti\n");
				break;
			case 2:
				ultima[turno]->destra=(Caverna *) malloc(sizeof(Caverna));
				cav=ultima[turno];
				ultima[turno]=ultima[turno]->destra;
				ultima[turno]->indietro=cav;
				printf("Hai creato la caverna a destra\n");
				break;
			case 3:
				ultima[turno]->sinistra=(Caverna *) malloc(sizeof(Caverna));
				cav=ultima[turno];
				ultima[turno]=ultima[turno]->sinistra;
				ultima[turno]->indietro=cav;
				printf("Hai creato la caverna a sinistra\n");
				break;
			default:
				printf("Non hai inserito una delle opzioni disponibili, la caverna verrà creata avanti(0)\n");
				cav=ultima[turno];
				ultima[turno]->avanti=(Caverna *) malloc(sizeof(Caverna));
				cav=ultima[turno];
				ultima[turno]=ultima[turno]->avanti;
				ultima[turno]->indietro=cav;
				printf("Hai creato la caverna avanti\n");
				break;
		}
	}
	printf("Inserisci lo stato della caverna (normale=1,speciale=2,accidentata=3)\n");
	scanf("%d",&stato);
	switch(stato){
		case 1:
			ultima[turno]->stato=enum_stato(0);
			break;
		case 2:
			ultima[turno]->stato=enum_stato(1);
			break;
		case 3:
			ultima[turno]->stato=enum_stato(2);
			break;
		default:
			printf("Non hai inserito una delle opzioni disponibili, la caverna verrà impostata a normale(1)\n");
			ultima[turno]->stato=enum_stato(0);
	}
	ultima[turno]->melassa=enum_melassa(casuale(50,30));
	ultima[turno]->imprevisto=enum_imprevisto(casuale(50,35));
	num_caverne[turno]++;
	printf("===FINE CREAZIONE CAVERNA===\n");
}

static void canc_caverna(){ //Cancella l'ultima caverna inserita
	if(partenza[turno]!=ultima[turno]){
		Caverna *cav=(Caverna *) malloc(sizeof(Caverna));
		cav=ultima[turno]->indietro;
		free(ultima[turno]);
		ultima[turno]=cav;
		ultima[turno]->avanti=NULL;
		ultima[turno]->destra=NULL;
		ultima[turno]->sinistra=NULL;
		num_caverne[turno]--;
		printf("L'ultima caverna inserita è stata cancellata\n");
	}
	else{
		printf("Non hai inserito ancora caverne, non puoi cancellare la caverna di partenza\n");
	}
}

static void stampa_cunicolo(){
	Caverna *cav=(Caverna *) malloc(sizeof(Caverna));
	cav=ultima[turno];
	printf("===I cunicoli verranno stampati dall'ultima inserito fino alla partenza===\n");
	for(int i=0;i<num_caverne[turno];i++){//scorre tutte le caverne fino ad arrivare all'ultima
		printf("Numero caverna:%d\n",num_caverne[turno]-i);
		printf("Quantità melassa:%d\n",cav->melassa);
		printf("Tipo imprevisto:%d\n",cav->imprevisto);
		printf("Stato caverna:%d\n",cav->stato);
		printf("=========================\n");
		cav=cav->indietro;
	}
	printf("Caverna di partenza:\n");
	printf("Quantità melassa:%d\n",cav->melassa);
	printf("Tipo imprevisto:%d\n",cav->imprevisto);
	printf("Stato caverna:%d\n",cav->stato);
	printf("===STAMPA TERMINATA===\n");
}

static void chiudi_cunicolo(){
	mappa_terminata[turno]=1;
	printf("===LA MAPPA DEL GIOCATORE %d E' STATA COMPLETATA===\n",turno);
	mappe_terminate=1;
	for(int x=0;x<num_giocatori;x++){
		if(mappa_terminata[x]==0){
			mappe_terminate=0;
		}
	}
}

extern void cancella_cunicoli(){ //Cancella la mappa creata precedentemente
	Caverna *cav=(Caverna *) malloc(sizeof(Caverna));
	cav=NULL;
	for(int j=0;j<num_giocatori;j++){
		while(partenza[j]!=ultima[j]){
			printf("Giocatore %d:Caverna cancellata\n",j);
			cav=ultima[j];
			ultima[j]=ultima[j]->indietro;
			free(cav);
			num_caverne[j]--;
		}
	}
	printf("===MAPPA CANCELLATA===\n");
	for(int x=0;x<num_giocatori;x++){
		mappa_terminata[x]=0;
	}
	mappe_terminate=0;
}
/*
 * SEZIONE GIOCA()
 * */

extern void gioca(){
	int scelta=0;
	char tasto;
	if(mappe_terminate==1){
		printf("===FASE GIOCO INIZIATA===\n");
		turno=0;
		while(vittoria[((turno) % num_giocatori)]==0){
			if(!(scavatrice[turno % num_giocatori]->melassa_energia==0 && scavatrice[turno % num_giocatori]->melassa_serbatoio==0) && uscito[turno % num_giocatori]==0 && sconfitto[turno % num_giocatori]==0){
				printf("===TURNO GIOCATORE N. %d===\n",turno % num_giocatori);
				printf("Serbatoio melassa energia: %d\n",scavatrice[turno % num_giocatori]->melassa_energia);
				printf("Serbatoio melassa raccolta: %d\n",scavatrice[turno % num_giocatori]->melassa_serbatoio);
				printf("=========================\n");				
				if(ostacolo[turno % num_giocatori]==0){
					printf("Scegli una tra le seguenti mosse:\n1)Avanza nel cunicolo\n2)Abbatti una parete\n3)Aggira l'ostacolo\n4)Esci dalla grotta\n5)Sposta melassa\n*Qualsiasi altro comando non è valido*\n");
					if(scanf("%d",&scelta)==0){
						printf("Hai inserito un carattere invece che un numero, gioco interroto,riavviare\n");
						break;
					}
					else{
						switch (scelta) {
							case 1:
								printf("===AVANZA===\n");
								avanza();
								printf("===AVANZAMENTO FINITO===\n");
								break;
							case 2:
								printf("===ABBATTI===\n");
								abbatti();
								printf("===ABBATTIMENTO FINITO===\n");
								break;
							case 3:
								printf("===AGGIRA===\n");
								if(ostacolo[turno % num_giocatori]==1){
									aggira();
								}
								else{
									printf("Non è presente nessun ostacolo da aggirare, turno perso\n");
								}
								printf("===AGGIRAMENTO FINITO===\n");
								break;
							case 4:
								printf("===ESCI===\n");
								esci();
								break;
							case 5:
								printf("===SPOSTA MELASSA===\n");
								sposta_melassa();
								break;
							default:
								printf("Comando non valido\n");		
								break;
						}
					}
				}
				else{
					printf("Sei bloccato da un ostacolo, l'unica opzione disponibile è aggirarlo\n");
					printf("===AGGIRA===\n");
					printf("Premi un tasto qualunque(1 solo) e poi invio per aggirare\n");
					scanf(" %c",&tasto);
					aggira();
					printf("===AGGIRAMENTO FINITO===\n");
				}
				if(scavatrice[turno % num_giocatori]->melassa_energia==0 && scavatrice[turno % num_giocatori]->melassa_serbatoio==0){
					printf("===HAI FINITO LE MELASSE, HAI PERSO===\n");
					sconfitto[turno % num_giocatori]=1;
				}
				prob_scontro=3*turno;
				if(casuale3(prob_scontro)==1 && controllo_unico()==0 && !(scavatrice[turno % num_giocatori]->melassa_energia==0 && scavatrice[turno % num_giocatori]->melassa_serbatoio==0) && uscito[turno % num_giocatori]==0 && sconfitto[turno % num_giocatori]==0){
					printf("Due scavatrici si sono incontrate, inizia lo scontro!\n");
					scontro();
				}
				printf("===TURNO FINITO===\n");
			}
			controllo_vittoria();
			turno++;
		}
		for(int i=0;i<num_giocatori;i++){
			if(vittoria[i]==1){	
				printf("===COMPLIMENTI GIOCATORE N.%d HAI VINTO!===\n",i);
			}
		}
		cancella_cunicoli();
	}
	else{
		printf("===PER GIOCARE BISOGNA PRIMA CREARE LA MAPPA===\n");
	}
	inizializzazione();
}

static void avanza(){
	int avanzamento=0,scelta=0;
	if(scavatrice[turno % num_giocatori]->pos->avanti!=NULL || scavatrice[turno % num_giocatori]->pos->destra!=NULL || scavatrice[turno % num_giocatori]->pos->sinistra!=NULL){
		printf("Direzioni disponibili per avanzamento:\n");
		if(scavatrice[turno % num_giocatori]->pos->avanti!=NULL){
			printf("-Avanti\n");
		}
		if(scavatrice[turno % num_giocatori]->pos->destra!=NULL){
			printf("-Destra\n");
		}
		if(scavatrice[turno % num_giocatori]->pos->sinistra!=NULL){
			printf("-Sinistra\n");
		}
		printf("In quali di queste direzioni vuoi andare? (1 per avanti,2 per destra e 3 per sinistra)\n");
		if(scanf("%d",&scelta)==0){
			printf("Hai inserito un carattere invece che un numero, abbattimento interroto\n");
		}
		switch(scelta){
			case 1:
				if(scavatrice[turno % num_giocatori]->pos->avanti!=NULL){
					scavatrice[turno % num_giocatori]->pos->avanti->indietro=scavatrice[turno % num_giocatori]->pos;
					scavatrice[turno % num_giocatori]->pos=scavatrice[turno % num_giocatori]->pos->avanti;
					printf("La scavatrice è avanzata in avanti\n");
					avanzamento=1;
				}
				else{
					printf("Direzione non disponibili,avanzamento interroto\n");
				}
				break;
			case 2:
				if(scavatrice[turno % num_giocatori]->pos->destra!=NULL){
					scavatrice[turno % num_giocatori]->pos->destra->indietro=scavatrice[turno % num_giocatori]->pos;
					scavatrice[turno % num_giocatori]->pos=scavatrice[turno % num_giocatori]->pos->destra;
					printf("La scavatrice è avanzata a destra\n");
					avanzamento=1;
				}
				else{
					printf("Direzione non disponibili,avanzamento interroto\n");
				}
				break;
			case 3:
				if(scavatrice[turno % num_giocatori]->pos->sinistra!=NULL){
					scavatrice[turno % num_giocatori]->pos->sinistra->indietro=scavatrice[turno % num_giocatori]->pos;
					scavatrice[turno % num_giocatori]->pos=scavatrice[turno % num_giocatori]->pos->sinistra;
					printf("La scavatrice è avanzata a sinistra\n");
					avanzamento=1;
				}
				else{
					printf("Direzione non disponibili,avanzamento interroto\n");
				}
				break;
			default:
				printf("Opzione non disponibile, avanzamento interrotto\n");
				break;
		}
		if(avanzamento==1){
			effetto_avanzamento();
		}
	}
	else{
		printf("Sei alla fine del cunicolo, non puoi avanzare da nessuna parte, devi per forza abbattere una parete\n");
		abbatti();
	}
}

static void effetto_avanzamento(){
	int melassa=0;
	switch(scavatrice[turno % num_giocatori]->pos->stato){
		case speciale:
			printf("La caverna è di tipo speciale, guadagni 1 melassa nel serbatoio di energia\n");
			scavatrice[turno % num_giocatori]->melassa_energia++;
			break;
		case accidentata:
			printf("La caverna è di tipo accidentata, perdi 1 melassa nel serbatoio di energia\n");
			scavatrice[turno % num_giocatori]->melassa_energia--;
			break;
		case uscita:
			printf("===La caverna è di tipo USCITA, al prossimo turno potrai uscire scegliendo l'opzione ESCI!===\n");
			break;
		case normale:
			printf("La caverna è di tipo normale, non succede niente\n");
			break;
	}
	switch(scavatrice[turno % num_giocatori]->pos->imprevisto){
		case nessun_imprevisto:
			printf("Non c'è stato nessun imprevisto, non succede niente\n");
			break;
		case crollo:
			printf("===Crollo improvviso!===\nPerdi 1 una melassa dal serbatoio di energia per liberarti. Per poter superare l'ostacolo usa l'opzione aggira il prossimo turno\n");
			ostacolo[turno % num_giocatori]=1;
			scavatrice[turno % num_giocatori]->pos=scavatrice[turno % num_giocatori]->pos->indietro;
			break;
		case baco:
			printf("===Imprevisto!C'è un baco!===\nIl serbatoio di energia e di raccolta sono stati svuostati dal baco\n");
			scavatrice[turno % num_giocatori]->melassa_energia=0; 
			scavatrice[turno % num_giocatori]->melassa_serbatoio=0;
			break;
	}
	switch(scavatrice[turno % num_giocatori]->pos->melassa){
		case nessuna:
			printf("Nella caverna non è presente melassa, non raccogli niente\n");
			melassa=0;
			break;
		case poca:
			printf("Nella caverna è presente poca melassa, raccogli 1 melassa\n");
			melassa=1;
			break;
		case molta:
			printf("Nella caverna è presente molta melassa, raccogli 3 melasse\n");
			melassa=3;
			break;
	}
	if(melassa>0){
		int scelta=0;
		printf("In che serbatoio vuoi aggiungere melassa? Digita 1 per il serbatoio di energia o 2 per quello del raccolto\n");
		if(scanf("%d",&scelta)==0){
				printf("Hai inserito un carattere invece che un numero, raccolta interrota, non raccogli più nessuna melassa\n");
		}
		switch(scelta){
			case 1:
				printf("Hai scelto il serbatoio di energia!\n");
				scavatrice[turno % num_giocatori]->melassa_energia=scavatrice[turno % num_giocatori]->melassa_energia+melassa;
				break;
			case 2:
				printf("Hai scelto il serbatoio di raccolta!\n");
				scavatrice[turno % num_giocatori]->melassa_serbatoio=scavatrice[turno % num_giocatori]->melassa_serbatoio+melassa;
				break;
			default:
				printf("Hai inserito un numero non valido, non raccogli più nessuna melassa\n");
				break;
		}
	}
}

static void abbatti(){
	int scelta=0,accidentata=20,normale=0,speciale=0,uscita=0;
	if(scavatrice[turno % num_giocatori]->pos->avanti==NULL || scavatrice[turno % num_giocatori]->pos->destra==NULL || scavatrice[turno % num_giocatori]->pos->sinistra==NULL){
		printf("Direzioni disponibili per abbattere un muro:\n");
		if(scavatrice[turno % num_giocatori]->pos->avanti==NULL){
			printf("-Avanti\n");
		}
		if(scavatrice[turno % num_giocatori]->pos->destra==NULL){
			printf("-Destra\n");
		}
		if(scavatrice[turno % num_giocatori]->pos->sinistra==NULL){
			printf("-Sinistra\n");
		}
		uscita=turno*5;
		speciale=normale=(100-uscita-accidentata)/2;
		printf("In quali di queste direzioni vuoi andare? (1 per avanti,2 per destra e 3 per sinistra)\n");
		if(scanf("%d",&scelta)==0){
			printf("Hai inserito un carattere invece che un numero, abbattimento interroto\n");
		}
		switch(scelta){
			case 1:
				if(scavatrice[turno % num_giocatori]->pos->avanti==NULL){
					printf("Hai scelto di abbattere la parete avanti!\n");
					scavatrice[turno % num_giocatori]->pos->avanti=(Caverna *) malloc(sizeof(Caverna));
					scavatrice[turno % num_giocatori]->pos->avanti->stato=enum_stato(casuale2(normale,speciale,accidentata));
					scavatrice[turno % num_giocatori]->pos->avanti->melassa=enum_melassa(casuale(40,40));
					scavatrice[turno % num_giocatori]->pos->avanti->imprevisto=enum_imprevisto(casuale(40,40));
				}
				else{
					printf("Direzione non disponibili,abbattimento interroto\n");
				}
				break;
			case 2:
				if(scavatrice[turno % num_giocatori]->pos->destra==NULL){
					printf("Hai scelto di abbattere la parete a destra!\n");
					scavatrice[turno % num_giocatori]->pos->destra=(Caverna *) malloc(sizeof(Caverna));
					scavatrice[turno % num_giocatori]->pos->destra->stato=enum_stato(casuale2(normale,speciale,accidentata));
					scavatrice[turno % num_giocatori]->pos->destra->melassa=enum_melassa(casuale(40,40));
					scavatrice[turno % num_giocatori]->pos->destra->imprevisto=enum_imprevisto(casuale(40,40));
				}
				else{
					printf("Direzione non disponibili,abbattimento interroto\n");
				}
				break;
			case 3:
				if(scavatrice[turno % num_giocatori]->pos->sinistra==NULL){
					printf("Hai scelto di abbattere la parete a sinsitra!\n");
					scavatrice[turno % num_giocatori]->pos->sinistra=(Caverna *) malloc(sizeof(Caverna));
					scavatrice[turno % num_giocatori]->pos->sinistra->stato=enum_stato(casuale2(normale,speciale,accidentata));
					scavatrice[turno % num_giocatori]->pos->sinistra->melassa=enum_melassa(casuale(40,40));
					scavatrice[turno % num_giocatori]->pos->sinistra->imprevisto=enum_imprevisto(casuale(40,40));
				}
				else{
					printf("Direzione non disponibili,abbattimento interroto\n");
				}
				break;
			default:
				printf("Opzione non disponibile, abbattimento interrotto\n");
				break;
		}
		scavatrice[turno % num_giocatori]->melassa_energia--;
	}
	else{
		printf("Non ci sono direzioni disponibili da abbattere, turno perso\n");
	}
}

static void aggira(){
	Caverna *cav=(Caverna *) malloc(sizeof(Caverna));	//crea una caverna che posizionerà tra la corrente e la successiva per la quale si è verificato il crollo
	cav->melassa=enum_melassa(casuale(50,30));
	cav->imprevisto=enum_imprevisto(casuale(50,35));
	cav->stato=enum_stato(casuale(33,33));
	if(scavatrice[turno % num_giocatori]->pos->avanti!=NULL){
		cav->avanti=scavatrice[turno % num_giocatori]->pos->avanti;	//salva la caverna successiva, come caverna avanti a quella appena creata
		cav->indietro=scavatrice[turno % num_giocatori]->pos;
		scavatrice[turno % num_giocatori]->pos=cav;	//sposta la scavatrice direttamente nella caverna creata
	}
	if(scavatrice[turno % num_giocatori]->pos->destra!=NULL){
		cav->avanti=scavatrice[turno % num_giocatori]->pos->destra;
		cav->indietro=scavatrice[turno % num_giocatori]->pos;	
		scavatrice[turno % num_giocatori]->pos=cav;	
	}
	if(scavatrice[turno % num_giocatori]->pos->sinistra!=NULL){
		cav->avanti=scavatrice[turno % num_giocatori]->pos->sinistra;
		cav->indietro=scavatrice[turno % num_giocatori]->pos;	
		scavatrice[turno % num_giocatori]->pos=cav;	
	}
	ostacolo[turno % num_giocatori]=0;
	effetto_avanzamento();
	cav->avanti->imprevisto=enum_imprevisto(casuale(50,35));
}

static void esci(){
	int fine_gioco=1,max=scavatrice[0]->melassa_serbatoio,max_ind=0;
	if(scavatrice[turno % num_giocatori]->pos->stato==uscita){
		printf("===CONGRATULAZIONI,SEI USCITO DALLA GROTTA!===\n===SE CI SONO ANCORA GIOCATORI VIVI NELLA GROTTA,ASPETTA LA FINE DEL GIOCO PER SCOPRIRE SE HAI VINTO!===\n");
		uscito[(turno % num_giocatori)]=1;
	}
	else{
		printf("Non sei in una uscita, turno perso\n");
	}
	for(int i=0;i<num_giocatori;i++){
		if(uscito[i]==0 && sconfitto[i]==0){
			fine_gioco=0;
		}
	}
	if(fine_gioco==1){
		for(int x=0;x<num_giocatori;x++){
			if(uscito[x]==1 && scavatrice[x]->melassa_serbatoio>max){
				max=scavatrice[x]->melassa_serbatoio;
				max_ind=x;
			}
		}
		printf("===IL GIOCATORE USCITO CON PIU' MELASSE E' IL N.:%d ===\n",max_ind);
		printf("===CON %d MELLASE NEL SERBATOIO DEL RACCOLTO ===\n",max);
		vittoria[max_ind]=1;
		
	}
	else{
		printf("Ci sono ancora giocatori vivi nella grotta, aspetta che escano o che finiscano le melasse\n");
	}
}

static void sposta_melassa(){
	int scelta=0,num_melasse=0;
	printf("Digita 1 se vuoi spostare la melassa dal serbatoio del raccolto a quello della energia, digita 2 per il contrario\n");
	if(scanf("%d",&scelta)==0){
		printf("Hai inserito un carattere invece che un numero, spostamento interrotto\n");
	}
	else{
		if(scelta==1 || scelta==2){
			printf("Quante melasse vuoi spostare?\n");
			if(scanf("%d",&num_melasse)==0){
			printf("Hai inserito un carattere invece che un numero, spostamento interrotto\n");
			}
			else{
				if(scelta==1){
					if(num_melasse<=scavatrice[turno % num_giocatori]->melassa_serbatoio){
						scavatrice[turno % num_giocatori]->melassa_serbatoio=scavatrice[turno % num_giocatori]->melassa_serbatoio-num_melasse;
						scavatrice[turno % num_giocatori]->melassa_energia=scavatrice[turno % num_giocatori]->melassa_energia+num_melasse;
					}
					else{
						printf("Stai cercando di spostare più melassa di quanta ne hai, spostamento interrotto\n");
					}
				}
				else{
					if(num_melasse<=scavatrice[turno % num_giocatori]->melassa_energia){
							scavatrice[turno % num_giocatori]->melassa_energia=scavatrice[turno % num_giocatori]->melassa_energia-num_melasse;
							scavatrice[turno % num_giocatori]->melassa_serbatoio=scavatrice[turno % num_giocatori]->melassa_serbatoio+num_melasse;
					}
					else{
						printf("Stai cercando di spostare più melassa di quanta ne hai, spostamento interrotto\n");
					}
				}
			}
			printf("Serbatoio melassa energia: %d\n",scavatrice[turno % num_giocatori]->melassa_energia);
			printf("Serbatoio melassa raccolta: %d\n",scavatrice[turno % num_giocatori]->melassa_serbatoio);
		}
		else{
			printf("Comando non valido, spostamento interrotto\n");
		}
	}		
}

/*
 * FINE SEZIONE GIOCA
 * */

extern void termina_gioco(){
	if(mappe_terminate==1){
		cancella_cunicoli();
	}
	for(int i=0;i<num_giocatori;i++){
		free(scavatrice[i]);
		free(partenza[i]);	
	}
	printf("===GIOCO TERMINATO===\n");
}

/*
 * SCONTRO TRA DUE SCAVATRICI
 * */

static void scontro(){
	int a=0,scelta=0,scelta1=0,esito=0,continua=0;
	do{
		continua=0;
		a=random_num(num_giocatori);
		//printf("a: %d, uscito: %d, sconfitto: %d\n",a,uscito[a],sconfitto[a]);
		if(uscito[a]==1 || sconfitto[a]==1 || a == turno % num_giocatori){
			continua=1;
		}
	}while(continua==1);
	printf("===INIZIO SCONTRO===\n");
	printf("===Scontro tra giocatore %d e %d===\n",turno % num_giocatori,a);
	printf("===SASSO CARTA FORBICE===\n");
	do{
		printf("Giocatore n.%d\nScegli tra sasso(0), carta(1), forbice(2)\n",turno % num_giocatori);
		scanf("%d",&scelta);
		printf("Giocatore n.%d\nScegli tra sasso(0), carta(1), forbice(2)\n",a);
		scanf("%d",&scelta1);
		if(scelta==scelta1){
			esito=0;
		}
		else{
			switch(scelta){
				case 0:
					if(scelta1==1){
						esito=2;
					}
					else{
						esito=1;
					}
					break;
				case 1:
					if(scelta1==0){
						esito=1;
					}
					else{
						esito=2;
					}
					break;
				case 2:
					if(scelta1==0){
						esito=2;
					}
					else{
						esito=1;
					}
					break;
				default:
					printf("Hai inserito un numero non valido\n");
					esito=2;
					break;
			}
			if(scelta1>2){
				printf("Hai inserito un numero non valido\n");
				esito=1;
			}
		}
	}while(esito==0);
	if(esito==1){
		printf("===GIOCATORE N.%d HAI VINTO!===\n===GIOCATORE N.%d HAI PERSO!===\n",turno%num_giocatori,a);
		sconfitto[a]=1;
	}
	else{
		printf("===GIOCATORE N.%d HAI VINTO!===\n===GIOCATORE N.%d HAI PERSO!===\n",a,turno%num_giocatori);
		sconfitto[turno%num_giocatori]=1;
	}
}

static int random_num(int b){
	int a=0;
	srand(time(NULL));
	a=rand()%b;	
	return a;
}

/*
 * SEZIONE CONTROLLI
 * */

static void controllo_vittoria(){
	if(vittoria[turno % num_giocatori]==0){
		vittoria[turno % num_giocatori]=1;
		for(int i=0;i<num_giocatori;i++){
			if(i!=turno % num_giocatori){	
				if(sconfitto[i]==0){
					vittoria[turno % num_giocatori]=0;
				}
			}
		}
	}
}

static int controllo_unico(){
	int unico=1;
	for(int i=0;i<num_giocatori;i++){
		if(sconfitto[i]==0 && uscito[i]==0 && i!=turno%num_giocatori){
			unico=0;
		}
	}
	return unico;
}

/*
 * SEZIONE ENUM
 * */
 
static int enum_melassa(int a){
	switch(a){
		case 0:
			return nessuna;
			break;
		case 1:
			return poca;
			break;
		case 2:
			return molta;
			break;
	}
	return nessuna;
}

static int enum_imprevisto(int a){
	switch(a){
		case 0:
			return nessun_imprevisto;
			break;
		case 1:
			return crollo;
			break;
		case 2:
			return baco;
			break;
	}
	return nessun_imprevisto;
}

static int enum_stato(int a){
	switch(a){
		case 0:
			return normale;
			break;
		case 1:
			return speciale;
			break;
		case 2:
			return accidentata;
			break;
		case 3:
			return uscita;
			break;
	}
	return normale;
}

/*
 * SEZIONE CASUALE
 * */

static int casuale(int a, int b) {
	int c=random_num(100);
    if (c <= a-1) {
		c=0;
    }
    else if (c >= a && c < a+b-1) {
		c=1;
    } 
    else {
          c=2;
    }
    return c;
}

static int casuale2(int a, int b,int c) {
	int d=random_num(100);
    if (d <= a-1) {
      d=0;
    }
    else if (d >= a && d < a+b-1) {
		d=1;
    } 
    else if (d >= a+b && d < a+b+c-1){
		d=2;
    }
    else{
		d=3;
    }
    return d;
}

static int casuale3(int a){
	int b=random_num(100);
	if(b<=a){
		b=1;
	}
	else{
		b=0;
	}
	return b;
}
