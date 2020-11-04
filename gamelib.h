enum quantita_melassa{nessuna,poca,molta=3} melassa;
enum tipo_imprevisto{nessun_imprevisto,crollo,baco} imprevisto;
enum tipo_caverna{normale,speciale,accidentata,uscita} stato;

typedef struct s__Caverna{
	struct s__Caverna * avanti;
	struct s__Caverna * destra;
	struct s__Caverna * sinistra;
	enum quantita_melassa melassa;
	enum tipo_imprevisto imprevisto;
	enum tipo_caverna stato;
} Caverna;

typedef struct s__Scavatrice{
	struct s__Caverna * pos;
	short int melassa_energia;
	short int melassa_serbatoio;
	int turno;
} Scavatrice;

extern void clean_stdin();
extern void chiudi_cunicoli();
extern void crea_cunicoli();
extern void gioca();
extern void insI();
extern void uscita_programma();
