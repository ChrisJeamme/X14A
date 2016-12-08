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

int main(int argc, char* argv[])
{
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