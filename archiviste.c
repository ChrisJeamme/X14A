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


/*********************************/
/*        Coding Party           */
/*      8/9 décembre 2016        */
/*      JEAMME Christopher       */
/*       BRUYERE Dimitri         */
/*********************************/

struct contenu
{
    long type;
    char texte[1000];
};

int main(int argc, char* argv[])
{    
    // printf("%s %s %s",argv[0],argv[1],argv[2]);

    /*Vérification du nombre d'arguments*/
        if (argc != 3)  //Si il n'y a pas 2 arguments
        {
            fprintf(stderr, "Erreur de nombre d'argument pour archiviste.c (argc = %d)\n", argc);
            exit(-1);
        }

    /* Récupération des arguments */
        int numero_ordre = atoi(argv[1]);
        int nombre_themes = atoi(argv[2]);

    /* Récupération file de messages*/

        /*Création d'une clé*/
        key_t cle = ftok("requete_journaliste", 'a');
        
        /*Création d'une file de message*/
        int id_file = msgget(cle, 0666 | IPC_CREAT | IPC_PRIVATE );
        if (id_file == -1)
        {
            perror("Erreur de création de la file de messages");
            exit(EXIT_FAILURE);
        }

        /*Réception des messages dans la file de messages*/
        while(1)
        {
            struct contenu message;
            int read = msgrcv(id_file, &message, 1000, 1, 0);
            if (read == -1)
            {
                perror("Erreur de lecture dans la file");
                exit(EXIT_FAILURE);
            }
            printf("archiviste : Message bien reçu : %s", message.texte);
        }


        printf("Salut ! Moi c'est le fils %d, ordre:%d nb_themes:%d\n", getpid(), numero_ordre, nombre_themes);

    while(1);

    exit(0);
}