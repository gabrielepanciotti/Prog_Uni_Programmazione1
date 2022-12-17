enum quantita_melassa{nessuna,poca,molta=3} melassa;
enum tipo_imprevisto{nessun_imprevisto,crollo,baco} imprevisto;
enum tipo_caverna{normale,speciale,accidentata,uscita} stato;

typedef struct Caverna{
	struct Caverna * avanti;
	struct Caverna * destra;
	struct Caverna * sinistra;
	struct Caverna * indietro; //Ho inserito un ulteriore parametro, per indicare la caverna precedente (Utile per cancellazione)
	enum quantita_melassa melassa;
	enum tipo_imprevisto imprevisto;
	enum tipo_caverna stato;
} Caverna;

typedef struct Scavatrice{
	struct Caverna * pos;
	short int melassa_energia;
	short int melassa_serbatoio;
} Scavatrice;

extern void crea_cunicoli();
extern void gioca();
extern void uscita_programma();
extern void cancella_cunicoli();
extern void termina_gioco();
