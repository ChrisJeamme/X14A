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


int main(int argc, char* argv[])
{
    /*Vérification du nombre d'arguments*/
        if (argc != 5)
        {
            fprintf(stderr, "Erreur de nombre d'argument pour journalistes.c");
            exit(-1);
        }
    
    int nb_archivistes = atoi(argv[1]);

    char requete = argv[2][0];
    int num_theme = atoi(argv[3]);  //Numéro du segment de mémoire contenant les articles du thème.

    if (requete == 'c')     //Consultation
    {
        int num_article = atoi(argv[4]);    //Numéro de l'article à consulter
    }

    if (requete == 'p')     //Publication
    {
        char texte_article[4];    //Texte de l'article
        strcpy(texte_article, argv[4]);
        printf("%s\n", texte_article);
    }

    if (requete == 'e')     //Effacement
    {
        int num_article = atoi(argv[4]);    //Numéro de l'article à effacer
    }

    return 0;
}