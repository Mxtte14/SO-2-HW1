//|------------------------------------|//
//| DICHIARAZIONE DELLE LIBRERIE USATE |//
//|------------------------------------|//
#include <stdio.h>
#include <utmp.h>           // Libreria per prendere alcuni dati dell'utente (Quando ha effettuato l'accesso, l'username)
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>           // Libreria utilizzata per gestire il tempo (idle, login time)
#include <string.h>         // Libreria utilizzata per lavorare con le stringhe
#include <stdlib.h>
#include <pwd.h>            // Libreria per prendere alcuni dati dell'utente (User directory, Shell directory, Nome reale, Room Number, Work Phone, Home Phone)
#include <stdbool.h>        // Libreria per lavorare con i valori booleani
#include <ctype.h>          // Importa funzioni per lavorare con i char (esempio: tolower())
// Librerie per ottenere informazioni di sistema (usata per ricavare l'idle time)
#include <sys/time.h>
#include <sys/stat.h>

// Struct utilizzata per l'idle time
#ifndef IDLE_H
#define IDLE_H

// Dichiarazione della struct
struct IdleTime
{
    time_t days;                // Salva i giorni di attivita' del sistema
    time_t hours;               // Salva le ore di attivita' del sistema
    time_t minutes;             // Salva i minuti di attivita' del sistema
    time_t seconds;             // Salva i secondi di attivita' del sistema
};

#endif /* IDLE_H */

// Struct utilizzata per la gestione dei dati di un utente
#ifndef USER_H
#define USER_H


// Dichiarazione della struct
typedef struct users_inf
{
    char user_name[UT_NAMESIZE + 1];            // Nickname
    char *real_name;                            // Nome Reale (GECOS prima informazione)
    char *usr_dir;                              // Directory dell'utente
    struct tm* login_time;                          // Quando ha fatto il login l'utente
    char *shell_dir;                            // Directory della shell utilizzata dall'utente
    char *room_number;                          // Room Number (GECOS seconda informazione)
    char *office;                               // Office Number (GECOS terza informazione)
    char *home;                                 // Home Number (GECOS quarta informazione)
    time_t last_mail;                           // Ultima mail arrivata
    time_t last_mail_read;                      // Ultima mail letta
    const char* timezone;
    // TTY
    char tty[UT_LINESIZE];
    char host[UT_HOSTSIZE];
    // Idle time
    struct IdleTime idle_time;                  // Idle time in base al tty in /dev/tty
    // Variabili di controllo
    bool exist;
    bool logged;
    // Variabile di controllo per indicare se printare o meno l'utente
    bool print;
} t_user;

#endif /* USER_H */

// Struct utilizzata per l'indentazione secondo il comando finger -s
#ifndef INDENT_H
#define INDENT_H

// Dichiarazione della struct
struct indent_s
{
    int ind_log;
    int ind_name;
    int ind_tty;
};

#endif /* INDENT_H */

// Strutc utilizzata per vedere quali opzioni del comando finger sono state specificate dall'utente
#ifndef OPTIONS_H
#define OPTIONS_H

// Dichiarazione della struct
struct options{
    bool l;     // Formattazione delle informazioni stampate
    bool s;     // Formattazione delle informazioni stampate
    bool p;     // Se attivo non controlla i file .plan .prrojects .pgpkey
    bool m;     // Se attivo non controlla se gli utenti specificati negli argomenti presi in input e' presente nel real name
};

#endif /* OPTIONS_H */