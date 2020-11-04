#include <stdio.h>
#include <stdlib.h>
#include "gamelib.h"

static Caverna * pStart_hartonen;
static Caverna * pLast_hartonen;
static Caverna * preLast_hartonen;

static Scavatrice * arvais;
static Scavatrice * hartonen;

static Caverna * pStart_arvais;
static Caverna * pLast_arvais;
static Caverna * preLast_arvais;

static int stato_creazione=0;
static int caverne_hartonen=1;
static int caverne_arvais=1;

extern void uscita_programma(){///funzione per terminare l'esecuzione del programma
    exit(1);
}

static void verifica(){///funzione per verificare chi ha vinto e infine esce dal programma
	if(hartonen->turno==-2 && arvais->turno>-2){
		printf("Vince Arvais\nGAME OVER\n");
		uscita_programma();
	}
	if(hartonen->turno>-2 && arvais->turno==-2){
		printf("Vince Hartonen\nGAME OVER\n");
		uscita_programma();
	}
	if(hartonen->turno==-1 && arvais->turno==-1){
		if(arvais->melassa_serbatoio == hartonen->melassa_serbatoio){
			printf("Parita'\nGAME OVER\n");
			uscita_programma();
		}else if(arvais->melassa_serbatoio > hartonen->melassa_serbatoio){
			printf("Vince Arvais\nGAME OVER\n");
			uscita_programma();
		}else{
			printf("Vince Hartonen\nGAME OVER\n");
			uscita_programma();
		}
	}
}

extern void clean_stdin(){///funzione per "pulire" l'stdin
    scanf("%*[^\n]");
    getchar();
}

static void morte(Scavatrice *s){///setto la variabile turno a meno due se il giocatore perde
	s->turno=-2;
}

static int check_melassa(Scavatrice *s){///funzone per controllare lo stato della mellassa
	if(s->melassa_energia<0){///se la melassa scende sotto zero si invoca la funzione morte
		morte(s);
		return 0;
	}else if(s->melassa_energia==0){///se la melassa è uguale a zero si ritorna il valore 1 altrimenti 2
		return 1;
	}else{
		return 2;
	}
}

static void esci(Scavatrice *s){///funzione per poter uscire
	if(s->pos->stato==uscita){///controllo se la scavatrice è in una caverna con stato uscita
		s->turno=-1;///se lo stato di uscita è presente allora il turno viene impostato a -1
	}else{
		printf("Non puoi uscire\n");///in caso negativo viene stampato a viene stampato a video che non si puo uscire
	}
}

static void scontro(){///lo scontro consiste in un semplice lancio di una moneta
	if(arvais->turno<0 || hartonen->turno<0){///si controlla se uno dei due giocatori non sia morto o uscito
		printf("Scontro non disponibile\n");///se uno dei due giocatori risulta morto o uscito non si potrà procedere con lo scontro
	}else{
		printf("Scontro, si lancia una moneta\n");
		int c=rand()%100;
		if(c<=49){
			hartonen->turno=-1;
			arvais->turno=-2;
		}else{
			arvais->turno=-1;
			hartonen->turno=-2;
		}
	}
}

static void casuale_scontro(int turno){///funzione per stabiliere se le due scavatrici si sono incontrate
	int c=rand()%100;
	if(c<=turno*3){
		scontro();
	}
}

static void modify_melassa(Scavatrice *s, short int b, short int c){///funzione per la modifica dello stato della melassa
	if(c==0){///la variabile c identifica il "tipo" di modifica da apportare alla melassa 0.tipo_imprevisto 1.tipo_caverna 2.quantita_melassa
		if(b==0){///la variabile b se è uguale a 0 e contemporaneamente lo è anche c allora si tratta di un imprevisto di tipo "baco"
			s->melassa_serbatoio=0;
			s->melassa_energia=0;
		}else{
			s->melassa_energia+=b;
		}
	}else if(c==1){///la variabile b identifica la qunatità di melassa da modificare per es. se si passa alla funzione +1 la melassa verrà incrementata di una unità
		s->melassa_serbatoio+=b;
	}else{///se la variabile c assume un valore diverso da 0 e 1 per es. 2 allora si darà all'utente di scegliere in quale serbatoio metterre la melassa trovata
		printf("Scegli dove mettere questa melassa 1)Serbatoio energia 2)Serbatoio raccolta\nQuantita': %d\n", b);
		scanf("%hd", &c);
		switch (c) {
			case 1:
				s->melassa_energia+=b;
				break;
			case 2:
				s->melassa_serbatoio+=b;
				break;
		}
	}
	if(s->melassa_serbatoio>10){///se la melassa supera il limite di 10 e/o 4 questa viene reimpostata al massimo indiciato dalle specifiche indicate dal gioco
		s->melassa_serbatoio=10;
	}
	if(s->melassa_energia>4){
		s->melassa_energia=4;
	}
}

static int casuale_stato(int turno){///funzione random che calcola come impostare il tipo di caverna
	int c=rand()%100;
	if (turno > 16) {///se il turno eccede dal valore 16 e quindi sforare l'80 percento viene reimpostato a 16
	  turno = 16;
	}
	if (c <= 20) {
	  return accidentata;
	} else if (c <= ((turno * 5) + 20)) {
	  return uscita;
	} else if (c >= (79 - (turno * 5))) {
	  return normale;
	} else {
	  return speciale;
	}
}

static int casuale(int a, int b, int c) {///funzoine random per impostare casualmente la quantita_melassa o tipo_imprevisto
  if (c == 0) {///le variabili "a" e "b" sono per le percentuali es. a=40, b=40 -> pecenruali saranno del 40%/40%/20%
		c=rand()%100;///la variabile "c" serve a stabilire se la generazione casuale deve essere di melassa o imprevisto
    if (c <= a-1) {
      c=nessuna;
    } else {
      if (c >= a && c < a+b-1) {
        c=poca;
      } else {
        if (c >= a+b) {
          c=molta;
        }
      }
    }
  } else {
		c=rand()%100;
    if (c <= a-1) {
			c=nessun_imprevisto;
    } else {
      if (c >= a && c < a+b-1) {
        c=crollo;
      } else {
        if (c >= a+b) {
           c=baco;
        }
      }
    }
  }
	return c;
}

static void salta_turno(Scavatrice *s){///funzione per incrementare il turno di 1 passando la scavatrice desiderata
	s->turno+=1;
}

static Caverna * genera_caverna(int stato_i, int melassa_i, int imprevisto_i){///funzione generica per l'allocazione di una caverna
	Caverna *p = (Caverna *) malloc(sizeof(Caverna));
	p->stato=stato_i;
	p->imprevisto=imprevisto_i;
	p->melassa=melassa_i;
	return p;///ritorna una caverna con i paramentri passati alla funzione
}

static void abbatti(Scavatrice *s){
	short int d;
	printf("Inserici la direzione in cui vuoi abbattere 1)Avanti 2)Sinistra 3)Destra\n");
	scanf("%hd", &d);
	clean_stdin();
	if(check_melassa(s)==2){
		if(d>0 && d<=3){
			switch (d) {
				case 1:
					if(s->pos->avanti!=NULL){
						printf("Non abbattere in questa direzione\n");
						break;
					}
					s->pos->avanti=genera_caverna(casuale_stato(s->turno),casuale(40,40,0),casuale(40,40,1));
					salta_turno(s);
					break;
				case 2:
					if(s->pos->sinistra!=NULL){
						printf("Non abbattere in questa direzione\n");
						break;
					}
					s->pos->sinistra=genera_caverna(casuale_stato(s->turno),casuale(4,40,0),casuale(40,40,1));
					salta_turno(s);
					break;
				case 3:
					if(s->pos->destra!=NULL){
						printf("Non puoi abbattere in questa direzione\n");
						break;
					}
					s->pos->destra=genera_caverna(casuale_stato(s->turno),casuale(40,40,0),casuale(40,40,1));
					salta_turno(s);
					break;
			}
		}else{
			printf("Inserisci un input valido!");
		}
	}else{
		printf("Melassa insufficente per abbattere\n");
	}
}

static void aggira(Scavatrice *s,short int d){///funzione aggirra chiamata solo dalla funzione casuale_avanza
	Caverna *p = genera_caverna(casuale_stato(s->turno),casuale(50,30,0),casuale(50,35,1));
	switch (d) {
		case 1:
			p->avanti=s->pos->avanti;
			break;
		case 2:
			p->avanti=s->pos->sinistra;
			break;
		case 3:
			p->avanti=s->pos->destra;
			break;
		default:
			p->avanti=s->pos->avanti;
			break;
	}
}

extern void insI(){///funzione per inizializzare la prima caverna del gioco dei due giocatori
	hartonen = (Scavatrice *) malloc(sizeof(Scavatrice));
	arvais = (Scavatrice *) malloc(sizeof(Scavatrice));

	Caverna *p = (Caverna *) malloc(sizeof(Caverna));
	p->avanti=NULL;
	p->destra=NULL;
	p->sinistra=NULL;
	p->stato=1;
	Caverna *q = (Caverna *) malloc(sizeof(Caverna));
	q->avanti=NULL;
	q->destra=NULL;
	q->sinistra=NULL;

	hartonen->melassa_energia=4;
	hartonen->melassa_serbatoio=10;
	hartonen->turno=0;

	arvais->melassa_energia=4;
	arvais->melassa_serbatoio=10;
	hartonen->turno=0;

	pStart_hartonen = p;
	hartonen->pos=pStart_hartonen;
	pLast_hartonen = pStart_hartonen;

	pStart_arvais = q;
	arvais->pos=pStart_arvais;
	pLast_arvais = pStart_arvais;

}

static Caverna * ins(Caverna *pLast, int stato_i, int d){///funzione per l'inserimento di una nuova caverna
	Caverna *p = genera_caverna(stato_i, casuale(50,30,0), casuale(50,35,1));
	p->avanti=NULL;
	p->destra=NULL;
	p->sinistra=NULL;
	switch (d)
	{
		case 1:
			pLast->avanti = p;
			break;
		case 2:
			pLast->sinistra = p;
			break;
		case 3:
			pLast->destra = p;
			break;
		default:
			pLast->avanti = p;
			break;
	}
	return p;
}

static void canc_caverna(){///base allo stato di creazione eliminia l'ultima elemento della lista del giocatore 1 o 2
	Caverna * pStart=NULL;
	Caverna * preLast=NULL;
	Caverna * pLast=NULL;
	if(stato_creazione==0){
		if(caverne_hartonen>1){
			pStart=pStart_hartonen;
			preLast=preLast_hartonen;
			pLast=pLast_hartonen;
			caverne_hartonen--;
		}else{
			printf("Non puoi eliminare se non ci sono caverna\n");
		}
	}else if(stato_creazione==1){
		if(caverne_arvais>1){
			pStart=pStart_arvais;
			preLast=preLast_arvais;
			pLast=pLast_arvais;
			caverne_arvais--;
		}else{
			printf("Non puoi eliminare se non ci sono caverna\n");
		}
	}
	free (pLast);
	pLast = preLast;
	pLast->avanti = NULL;
	pLast->destra = NULL;
	pLast->sinistra = NULL;
	Caverna *pScan = pStart;
	while (pScan != NULL)
	{
		if (pScan->avanti != NULL)
		{
			if (pScan->avanti == pLast)
			{
				preLast = pScan;
				break;
			}
			pScan = pScan->avanti;
		}
		else
		{
			if (pScan->destra != NULL)
			{
				if (pScan->destra == pLast)
				{
					preLast = pScan;
					break;
				}
				pScan = pScan->destra;
			}
			else
			{
				if (pScan->sinistra != NULL)
				{
					if (pScan->destra == pLast)
					{
						preLast = pScan;
						break;
					}
					pScan = pScan->sinistra;
				}
				else
				{
					pScan = NULL;
				}
			}
		}
	}
}

static void crea(){///funzione per inserire le caverne con input preso dall'utente e
	int d;
	if(stato_creazione!=2){
		int stato_i;
		printf("Inserisci il tipo di della caverna 0)Normale 1)Speciale 2)Accidentata\n*Se inserisci qualsiasi altro valore il tipo sarà Normale*\n");
		if((scanf("%d", &stato_i))==0){
			stato_i=0;
			clean_stdin();
		}
			printf("Inserisci la direzione 1)Avanti 2)Sinistra 3)Destra\n*Se inserisci qualsiasi altro valore la direzione sarà Avanti*\n");
			scanf("%d",&d);
			clean_stdin();
			if(stato_creazione==0){
				Caverna *p=ins(pLast_hartonen,stato_i,d);
				preLast_hartonen = pLast_hartonen;
				pLast_hartonen = p;
				caverne_hartonen++;
			}else if(stato_creazione==1){
				Caverna *p=ins(pLast_arvais,stato_i,d);
				preLast_arvais = pLast_arvais;
				pLast_arvais = p;
				caverne_arvais++;
			}
	}else{
		printf("Sei sicuro di eliminare tutte le caverne già create? 0)Si 1)No\n*Se inserisci qualsiasi altro valore la direzione sarà Avanti*\n");
		scanf("%d", &d);
			if(d==0){
				stato_creazione=1;
				for(int i=1;i<caverne_hartonen;i++){
					canc_caverna();
				}
				stato_creazione=0;
				for(int i=1;i<caverne_hartonen;i++){
					canc_caverna();
				}
			}
	}
}

static void stampa_caverna(){///funzione per stampare le caverne passando come paramentro la caverna del giocatore desiderato
	int d;
	printf("Inserici un numero per stampare le caverne 1)Hartonen 2)Arvais\n**Se inserisci qualsiasi altro valore sarà stampato Arvais*\n");
	scanf("%d", &d);
	clean_stdin();
	Caverna * ca;
	if(d==1){
		ca=pStart_hartonen;
	}else{
		ca=pStart_arvais;
	}
	while(ca!=NULL){
		if(ca->avanti!=NULL){
			printf("Avanti/ Stato:%d",ca->avanti->stato);
			printf(" / Imprevisto:%d /",ca->avanti->imprevisto);
			printf(" Melassa:%d /",ca->avanti->melassa);
			ca=ca->avanti;
		}else{
			if(ca->destra!=NULL){
				printf("Destra/ Stato:%d",ca->destra->stato);
				printf(" / Imprevisto:%d /",ca->destra->imprevisto);
				printf(" Melassa:%d /",ca->destra->melassa);
				ca=ca->destra;
			}else{
				if(ca->sinistra!=NULL){
					printf("Sinistra/ Stato:%d",ca->sinistra->stato);
					printf(" / Imprevisto:%d /",ca->sinistra->imprevisto);
					printf(" Melassa:%d /",ca->sinistra->melassa);
					ca=ca->sinistra;
				}else{
					ca=NULL;
				}
			}
		}
	}
}

extern void chiudi_cunicoli(){///setta la variabile globale stato_creazione
	if(stato_creazione==0){
		if(caverne_hartonen<10){
			printf("Devi inserire almeno 10 caverne\n");
		}else{
			stato_creazione=1;
		}
	}else if(stato_creazione==1){
		if(caverne_arvais<10){
			printf("Devi inserire almeno 10 caverne\n");
		}else{
			stato_creazione=2;
		}
	}
}

static short int verify(Scavatrice *s, short int d){///funzione per verificare se la direzione immessa dall'utente per avanzare è valida
	switch (d) {
		case 1:
			if(s->pos->avanti==NULL){
				break;
			}
			return 1;
		case 2:
			if(s->pos->sinistra==NULL){
				break;
			}
			return 2;
		case 3:
			if(s->pos->destra==NULL){
				break;
			}
			return 3;
		default:
			return 0;
	}
	return 0;
}

static void casuale_avanza(Scavatrice *s, short int d){///funzione per calcolare crolli imprevisti
	int c=rand()%100;
	if(c<=24){///se il crollo avviene allora viene chiamata la automaticamente la funzione aggira
		printf("\n*Crollo improvviso*\n");
		aggira(s,d);
	}
}

static void effetto(Scavatrice *s){///funzione per applicare gli "effetti" della caverna in cui si è avanzati
	switch(s->pos->stato){
		case speciale:///la caverna è "speciale" e quindi si richiama la funzione modify_melassa che aggiunge una unità di melassa
		  printf("***Speciale\n");
			modify_melassa(s,1,1);
			break;
		case accidentata:
			printf("***Accidentata\n");
			modify_melassa(s,-1,1);
			break;
		case uscita:
			printf("***Uscita");
			break;
		default:
			break;
	}
	switch(s->pos->imprevisto){
		case crollo:
			modify_melassa(s,-1,0);
			printf("***Crollo\n");
			break;
		case baco:
			printf("***Baco\n");
			modify_melassa(s,0,0);
			break;
		default:
			break;
	}
	switch(s->pos->melassa){
		case poca:
			modify_melassa(s,1,2);
			break;
		case molta:
			modify_melassa(s,3,2);
			break;
		default:
			break;
	}
}

static void avanza(Scavatrice *s){///funzione per avanzare in una caverna
	short int d;
	printf("Inserici la direzione in cui vuoi avanzare 1)Avanti 2)Sinistra 3)Destra\n");
	scanf("%hd", &d);
	clean_stdin();
	d=verify(s,d);///verifico se la direzione immessa è corretta a livello di puntatori
	switch (d) {
		case 0:
			printf("Non puoi avanzare in questa direzione\n");
			break;
		case 1:
			s->pos=s->pos->avanti;
			break;
		case 2:
			s->pos=s->pos->sinistra;
			break;
		case 3:
			s->pos=s->pos->destra;
			break;
	}
	casuale_avanza(s,d);
	effetto(s);
	salta_turno(s);
}

extern void crea_cunicoli(){
	int d;
	do {
		printf("\n\n1)Crea cunicoli 2)Rimuovi l'ultimo cunicolo 3)Stampa i cunicoli 4)Chiudi cunicoli\n\n");
		scanf("%d", &d);
		clean_stdin();
		switch (d) {
			case 1:
				crea();
				break;
			case 2:
					canc_caverna();
					break;
			case 3:
					stampa_caverna();
					break;
			case 4:
					chiudi_cunicoli();
					break;
		}
	} while(stato_creazione!=2);
}

extern void gioca(){
	Scavatrice *s;
	int d;
	if(stato_creazione==2){
		do {
			if( (hartonen->turno==arvais->turno && hartonen->turno>=0) || arvais->turno<0){
				s=hartonen;
			}else if((arvais->turno<hartonen->turno && arvais->turno>=0) || hartonen->turno<0){
				s=arvais;
			}else{
				verifica();
			}
			printf("\n\n1)Avanza 2)Abbatti 3)Aggira\n*Qualsiasi altro tasto ti fara' avanzare*\n");
			scanf("%d",&d);
			clean_stdin();
			switch (d) {
				case 1:
					avanza(s);
					break;
				case 2:
					abbatti(s);
					break;
				case 3:
					esci(s);
					break;
				default:
					avanza(s);
					break;
			}
			casuale_scontro(s->turno);
			verifica();
		} while(1);
	}else{
		printf("Non puoi giocare se non crei almeno 10 cunicoli per giocatore\n");
	}
}
