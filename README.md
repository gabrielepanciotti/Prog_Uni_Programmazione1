# Progetto-finale-Programmazione1-2020

Consegna e funzionamento programma su file pdf.

Ho aggiunto varie funzionalità,la principale riguarda il numero di giocatori che possono giocare, tramite l'assegnazione di valore ad una costante(num_giocatori) prima dell'esecuzione del codice si può impostare il numero di giocatori che parteciperanno.
Una volta avviato il gioco la prima cosa che si può fare è creare la mappa per ogni giocatore, il numero di caverne minime può essere impostato come per il numero di giocatori prima dell'esecuzione del programma.
Una volta creata la mappa per ogni giocatore si può iniziare a giocare o cancellare tutte le mappe per poi ricrearle.
Una volta iniziato a giocare, il gioco continua finchè rimane un solo giocatore in vita dentro la grotta o finchè tutti i giocatori sono o usciti dalla grotta o morti.
Se più di un giocatore esce dalla grotta quello con più melassa nel serbatoio vince,un altra funzionalità aggiunta è la possibilità di spostare la melassa da un serbatoio all'altro.
Una volta finito il gioco si deve ricreare una nuova mappa per poter rigiocare.
In caso di scontro si gioca a sasso,carta e forbice, chi vince sopravvive l'altro viene eliminato.

Per eseguire il codice:
gcc -c main.c -std="c11" -Wall && gcc -c gamelib.c -std="c11" -Wall && gcc -o game main.o gamelib.o -std="c11" -Wall && ./game
