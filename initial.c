#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


/*********************************/
/*        Coding Party           */
/*      8/9 décembre 2016        */
/*      JEAMME Christopher       */
/*       BRUYERE Dimitri         */
/*********************************/

/* Retourne aléatoirement le type de la demande d'archive */
/* 1 pour une consultation */
/* 2 pour une publication */
/* 3 pour une suppression */
int demande_archive()
{
    int nb = rand()%10 + 1;
    if (nb < 7)
        return 1;    //consultation
    if (nb < 10)
        return 2;   //publication
    return 3;       //effacement
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
    
    /*Vérification du nombre d'arguments*/
        if (argc != 3)
        {
            fprintf(stderr, "Usage : initial <nb_archivistes> <nb_themes>\n"
                            "\tnb_archivistes > 1\n"
                            "\tnb_themes >1\n\n");
            exit(-1);
        }

    /*On récupère les arguments*/
        int nb_archivistes = atoi(argv[1]);
        int nb_themes = atoi(argv[2]);

    /*Vérification des arguments*/
        if (nb_archivistes < 2 || nb_themes < 2)
        {
            fprintf(stderr, "Usage : initial <nb_archivistes> <nb_themes>\n"
                            "\tnb_archivistes > 1\n"
                            "\tnb_themes >1\n\n");
            exit(-1);
        }
    
    /*Création de l'ensemble de signaux à capter*/

        sigset_t ensemble_signaux;
        sigemptyset(&ensemble_signaux); //On créer un ensmble de signaux vide
        sigfillset(&ensemble_signaux);  //On met tout les signaux possibles

        //Vérification sûrement inutile
        if ( !sigismember(&ensemble_signaux, SIGKILL) || !sigismember(&ensemble_signaux, SIGCHLD))
        {
            fprintf(stderr, "Erreur: SIGKILL ou SIGCHLD n'est pas dans l'ensemble de signaux 'ensemble_signaux'");
            exit(-1);
        }
        
        //On supprime les signaux qui n'impliqueront pas la terminaison des archivistes et des journalistes
        sigdelset(&ensemble_signaux, SIGKILL);
        sigdelset(&ensemble_signaux, SIGCHLD);

    return 0;
}


