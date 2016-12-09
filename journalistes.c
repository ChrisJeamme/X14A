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

struct contenu
{
    long type;
    char requete;
    int theme;
    char texte[6];
    pid_t identite;
};

int main(int argc, char* argv[])
{
    /*Vérification du nombre d'arguments*/
        if (argc != 5)
        {
            fprintf(stderr, "Erreur de nombre d'argument pour journalistes.c");
            exit(-1);
        }
    
    printf("\nnb_archivistes %s \nrequete : %s \ntheme : %s \ntexte : %s\n\n",argv[1],argv[2], argv[3], argv[4]);

    int nb_archivistes = atoi(argv[1]);

    char requete = argv[2][0];
    int num_theme = atoi(argv[3]);  //Numéro du segment de mémoire contenant les articles du thème.
    key_t cle;

    if (requete == 'c' || requete == 'e')     //Consultation ou Effacement
    {
        /*Création d'une clé */
        cle = ftok("requete_journaliste", 1);

        /*Création d'une file de message*/
        int id_file = msgget(cle, 0666 | IPC_CREAT | IPC_PRIVATE);
        if (id_file == -1)
        {
            perror("Erreur de création de la file de messages");
            exit(EXIT_FAILURE);
        }

        /*Création du message*/
        struct contenu message;
        message.type=1;
        message.requete = requete;
        message.theme = num_theme;
        strcpy(message.texte, argv[4]);
        message.identite = getpid();
        int taille = sizeof(struct contenu)-1;

        /*Envoi du message*/
        int envoi = msgsnd(id_file, &message, taille+1, 0);
        if (envoi == -1)
        {
            perror("Erreur d'envoi du message");
            exit(EXIT_FAILURE);
        }
        printf("journaliste : Message bien envoyé \n");

        
        /* Il faudra vérifier si le numéro de l'article à consulter n'est pas supérieur au nombre d'articles du thème*/
        /* et avertir le journaliste si c'est le cas*/
    }

    if (requete == 'p')     //Publication
    {
        char texte_article[4];    //Texte de l'article
        strcpy(texte_article, argv[4]);


        /*Création d'une clé */
        cle = ftok("requete_journaliste", 'a');

        /*Création d'une file de message*/
        int id_file = msgget(cle, 0666 | IPC_CREAT | IPC_PRIVATE);
        if (id_file == -1)
        {
            perror("Erreur de création de la file de messages");
            exit(EXIT_FAILURE);
        }

        /*Création du message*/
        struct contenu message;
        message.type=1;
        message.requete = requete;
        message.theme = num_theme;
        strcpy(message.texte, argv[4]);
        message.identite = getpid();
        int taille = sizeof(struct contenu)-1;

        /*Envoi du message*/
        int envoi = msgsnd(id_file, &message, taille+1, 0);
        if (envoi == -1)
        {
            perror("Erreur d'envoi du message");
            exit(EXIT_FAILURE);
        }
        printf("journaliste : Message bien envoyé \n");

    }

    exit (0);
}