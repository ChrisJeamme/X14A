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
char* demande_archive();
void le_gros_sigaction();

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

    /* Vérification des arguments */
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
                //printf("Lancement du fils %d\n", pid);
                char* arguments[] = {"archiviste", "0", argv[2], NULL}; //Numéro d'ordre , Nombre de thèmes
                execve("archiviste", arguments, envp);
                while(1);
            }
            else            //Père
            {
                liste_pid[i]=pid;   //On stock le pid dans la liste
                //printf("%d\n",liste_pid[i]);
            }
        }

    /* Gestion des signaux de terminaison des fils */
        le_gros_sigaction(); //Ajout de la règle pour tous les signaux sauf SIGKILL et SIGCHLD

    /* Création des segments de mémoire partagé */
        // for(i=0; i<nb_themes; i++)
        // {

        // }

        int memoire_p;
        int* entier_p;
        key_t cle_smp = ftok ("smp", 'b');
        
        if((memoire_p = shmget(cle_smp, sizeof(int), IPC_CREAT | IPC_EXCL | 0660)) != -1)
        {
            if((entier_p = shmat(memoire_p, NULL, 0)))
            {
                *entier_p = 5;
                printf("Wesh wesh, je fou mon entier %d", *entier_p);
                shmdt(&entier_p);
            }
        }
        else
        {
            fprintf(stderr, "Problème de mémoire partagé (initial)\n");
            //exit(-1);
        }
        
        //shmctl(memoire_p, IPC_RMID, NULL);

        //TODO Détacher les SMP

    /* Création des journalistes */
        while(1)
        {
            pid_t pid = fork();
            char *requete = demande_archive();
            char *theme = "3";
            char *texte;
            if (requete[0]=='p')    //publication
            {
                texte = "zizi";     // texte de l'article
            }
            else 
            {
                texte = "12";     //numero de l'article
            }

            if(pid == -1)   //Erreur
            {
                fprintf(stderr, "Erreur de fork\n");
                exit(-1);
            }
            if(pid == 0)    //Fils
            {
                //printf("Lancement du fils %d\n", pid);
                char* arguments[] = {"journalistes", argv[1], requete, theme, texte,NULL}; //nombre d'archivistes, requete, theme de l'article, texte ou num_article
                execve("journalistes", arguments, envp);
                while(1);
            }
            else            //Père
            {
                liste_pid[i]=pid;   //On stock le pid dans la liste
                //printf("%d\n",liste_pid[i]);
                i++;
            }
            sleep(2);
        }

    return 0;  
}

void terminaison_fils(int signal)
{
    printf("Demande de terminaison des fils\n");
    int i;
    for(i=0; i<nb_archivistes; i++)
    {
        kill(liste_pid[i], SIGKILL);    //On envoi SIGTERM à tout les archivistes
    }
    exit(-1);
}

char* demande_archive()
{
    int nb = rand()%10 + 1;
    if (nb < 7)
        return "c";    //consultation
    if (nb < 10)
        return "p";   //publication
    return "e";       //effacement
}

void le_gros_sigaction()
{
    //Gestionnaire
    struct sigaction s_terminaison_fils;        //Création gestionnaire
    s_terminaison_fils.sa_handler = &terminaison_fils;   //Adresse fonction du gestionnaire
    s_terminaison_fils.sa_flags = 0;            //Ignoré
    sigemptyset(&s_terminaison_fils.sa_mask);   //Aucun signaux masqué => ensemble signal vide dans sa_mask

                /*//Création de l'ensemble de signaux à écouter
                sigset_t signaux;
                sigemptyset(&signaux);
                sigfillset(&signaux);
                if(!sigismember(&signaux, SIGKILL) || !sigismember(&signaux, SIGCHLD))
                {
                    fprintf(stderr, "Erreur improbable\n");
                    exit(-1);
                }
                sigdelset(&signaux, SIGKILL);
                sigdelset(&signaux, SIGCHLD);
                sigwaitinfo(&signaux, NULL);*/

    //Algorithme de pataras mais au moins on ne fait pas un seul for avec un if à chaque boucle !
    
    int i;
    for(i=1; i<9; i++)  //On coupe à SIGKILL
    {
        sigaction(i, &s_terminaison_fils, 0); //Ajout de la règle pour SIG**** = i
    }
    for(i=10; i<17; i++)   //On coupe à SIGCHLD
    {
        sigaction(i, &s_terminaison_fils, 0); //Ajout de la règle pour SIG**** = i
    }
    for(i=18; i<=32; i++)
    {
        sigaction(i, &s_terminaison_fils, 0); //Ajout de la règle pour SIG**** = i
    }
}