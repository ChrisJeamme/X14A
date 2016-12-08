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

int main(int argc, char* argv[])
{    printf("%s %s %s",argv[0],argv[1],argv[2]);

    /*Vérification du nombre d'arguments*/
        if (argc != 3)  //Si il n'y a pas 2 arguments
        {
            fprintf(stderr, "Erreur de nombre d'argument pour archiviste.c (argc = %d)\n", argc);
            exit(-1);
        }


    /* Récupération des arguments */
        int numero_ordre = atoi(argv[1]);
        int nombre_themes = atoi(argv[2]);

        printf("Salut ! Moi c'est le fils %d, ordre:%d nb_themes:%d", getpid(), numero_ordre, nombre_themes);

    //while(1);

    return 0;
}