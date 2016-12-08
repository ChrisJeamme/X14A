#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


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

void terminaison_fils(int signal);
char demande_archive();


pid_t *liste_pid;
int nb_archivistes;
int nb_themes;

int main(int argc, char* argv[], char* envp[])
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
        nb_archivistes = atoi(argv[1]);
        nb_themes = atoi(argv[2]);

    /*Vérification des arguments*/
        if (nb_archivistes < 2 || nb_themes < 2)
        {
            fprintf(stderr, "Usage : initial <nb_archivistes> <nb_themes>\n"
                            "\tnb_archivistes > 1\n"
                            "\tnb_themes >1\n\n");
            exit(-1);
        }

    /* Création des archivistes*/
        liste_pid = malloc(nb_archivistes * sizeof(pid_t)); //pour stocker le pid de chacun des archivistes
        int i;
        printf("\n\n%d\n\n", nb_archivistes);
        for(i=0; i<nb_archivistes; i++) 
        {
            pid_t pid = fork();  //Création de l'archiviste

            if(pid == -1)   //Erreur
            {
                fprintf(stderr, "Erreur de fork\n");
                exit(-1);
            }
            if(pid == 0)    //Fils
            {
                printf("fils (de pute)\n");

                char* arguments[] = {"0", argv[2], NULL}; //Numéro d'ordre , Nombre de thèmes
                execve("archiviste",arguments,envp);
                exit(-1);
            }
            else            //Père
            {
                liste_pid[i]=pid;   //On stock le pid dans la liste
                printf("%d\n",liste_pid[i]);
            }
        }
        
    /*Gestion des signaux de terminaison des fils*/

        struct sigaction s_terminaison_fils;        //Création règle
        s_terminaison_fils.sa_handler = &terminaison_fils;   //Adresse fonction gestionnaire de la règle
        s_terminaison_fils.sa_flags = 0;            //Ignoré
        sigemptyset(&s_terminaison_fils.sa_mask);   //Aucun signaux masqué => ensemble signal vide dans sa_mask
        sigaction(SIGKILL, &s_terminaison_fils, 0); //Ajout de la règle pour SIGKILL
        sigaction(SIGCHLD, &s_terminaison_fils, 0); //Ajout de la règle pour SIGCHLD
        
        while(1);

    return 0;  
}

void terminaison_fils(int signal)
{
    int i;
    for(i=0; i<nb_archivistes; i++)
    {
        kill(liste_pid[i], SIGTERM);    //On envoi SIGTERM à tout les archivistes
    }
}

char demande_archive()
{
    int nb = rand()%10 + 1;
    if (nb < 7)
        return 'c';    //consultation
    if (nb < 10)
        return 'p';   //publication
    return 'e';       //effacement
}
