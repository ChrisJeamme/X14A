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

#define MAX_ARTICLE 20

/*********************************/
/*        Coding Party           */
/*      8/9 décembre 2016        */
/*      JEAMME Christopher       */
/*       BRUYERE Dimitri         */
/*********************************/

void recup_tout_smp();
void recup_smp(char* fichier, int code);
int ajout_article(int numero_theme, char* article);
int suppr_article(int numero_theme, int numero_article);
void afficher_liste_themes();
void suppr_smp();

int nb_themes;
int memoire_p = -1;

struct contenu
{
    long type;
    char requete;
    int theme;
    char texte[6];
    pid_t identite;
};

typedef struct
{
  int numero;
  char article[MAX_ARTICLE][5];
}theme;
theme* liste_themes;


int main(int argc, char* argv[])
{
    /* Vérification du nombre d'arguments */
    printf("archiviste : Je suis le numero : %s\n\n", argv[1]);

        if (argc != 3)  //Si il n'y a pas 2 arguments
        {
            fprintf(stderr, "Erreur de nombre d'argument pour archiviste.c (argc = %d)\n", argc);
            exit(-1);
        }

    /* Récupération des arguments */
        int numero_ordre = atoi(argv[1]);
        nb_themes = atoi(argv[2]);

    /* Récupération ensembles de mémoire partagé */
        
        //recup_tout_smp();
        recup_smp("initial.c", 'z');   //Dans memoire_p

    /* Récupération file de messages*/

        /*Création d'une clé*/
        key_t cle = ftok("requete_journaliste", 1);
        
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
            printf("archiviste %d: Message bien reçu : \nrequete : %c\ntheme : %d\ntexte : %s\npid journaliste : %d\n",numero_ordre, message.requete, message.theme, message.texte, message.identite);

            /*Vérification*/
            
        }

    while(1);

    exit(0);
}

void recup_tout_smp()
{
  int i;
  char nom[3];
  for(i=1; i<=nb_themes; i++)
  {
    sprintf(nom,"%d",i);
    printf("On veut récupérer recup_smp(%s, %d)", nom, i);
    //recup_smp(nom,i);
  }
}

void recup_smp(char* fichier, int code)
{
  key_t cle_m = ftok(fichier, code);
  if (cle_m == -1)
  {
    perror("ftok");
    exit(EXIT_FAILURE);
  }
  
  char* charal;

  if((memoire_p = shmget(cle_m, 0, 0)) != -1)
  {
    if((charal = shmat(memoire_p, NULL, 0)))
    {
      printf("Le char que j'ai récupéré: %c\n", *charal);
      shmdt(charal);
    }
  }
  else
  {
      perror("smhget de recup a renvoyé -1...\n");
  }
}

int ajout_article(int numero_theme, char* article)
{
  int j;
  
  if(numero_theme < 1 || numero_theme > nb_themes)
  {
    fprintf(stderr,"Numero de thème incorrect\n");
    return -1;
  }

  if(liste_themes == NULL)
  {
    fprintf(stderr,"Liste de thème non initialisé\n");
    return -1;
  }
  
  for(j=1; strcmp(liste_themes[numero_theme].article[j],"VIDE")!=0 && j<MAX_ARTICLE; j++){;} //On parcourt jusqu'à tomber sur la prochaine place libre
  
  if(j == MAX_ARTICLE)  //Max atteint
  {
    fprintf(stderr,"Nombre d'article max atteint pour le thème %d", numero_theme);
  }
  else                    //Case vide trouvé
  {
    strcpy(liste_themes[numero_theme].article[j], article); //On place le nouvel article;
  }
  
  return j; //On renvoi le numéro de l'article dans le thème
}

int modif_article(int numero_theme, int numero_article, char* article)
{
  int j;

  if(numero_theme < 1 || numero_theme > nb_themes)
  {
    fprintf(stderr,"Numero de thème incorrect\n");
    return -1;
  }
  
  if(numero_article < 1 || numero_article > MAX_ARTICLE)
  {
    fprintf(stderr,"Numero d'article incorrect\n");
    return -1;
  }

  if(liste_themes == NULL)
  {
    fprintf(stderr,"Liste de thème non initialisé\n");
    return -1;
  }

  if(strcpy(liste_themes[numero_theme].article[numero_article], "VIDE"))
  {
    fprintf(stderr,"L'article est vide\n");
    return -1;
  }
  
  for(j=1; strcmp(liste_themes[numero_theme].article[j],"VIDE")!=0 && j<MAX_ARTICLE; j++){;} //On parcourt jusqu'à tomber sur la prochaine place libre
  
  printf("L'article %d du thème %d (%s) a été modifié par: ", numero_article, numero_theme, liste_themes[numero_theme].article[j]);

  strcpy(liste_themes[numero_theme].article[j], article); //On modifie l'article

  printf("%s", liste_themes[numero_theme].article[j]);

  return j; //On renvoi le numéro de l'article dans le thème
}

int suppr_article(int numero_theme, int numero_article)
{
  if(numero_theme < 1 || numero_theme > nb_themes)
  {
    fprintf(stderr,"Numero de thème incorrect\n");
    return -1;
  }
  
  if(numero_article < 1 || numero_article > MAX_ARTICLE)
  {
    fprintf(stderr,"Numero d'article incorrect\n");
    return -1;
  }

  if(liste_themes == NULL)
  {
    fprintf(stderr,"Liste de thème non initialisé\n");
    return -1;
  }

  if(strcpy(liste_themes[numero_theme].article[numero_article], "VIDE"))
  {
    fprintf(stderr,"L'article est déjà vide\n");
    return -1;
  }

  strcpy(liste_themes[numero_theme].article[numero_article], "VIDE");
  return 0;
}

void afficher_liste_themes()
{
  if(liste_themes == NULL)
  {
    perror("Liste de thèmes non initialisé\n");
    exit(EXIT_FAILURE);
  }

  int i, j;
  for(i=1; i<=nb_themes; i++)
  {
    for(j=1; j<MAX_ARTICLE; j++)
    {
      printf("Thème %d | Article %d: %s\n", i, j, liste_themes[i].article[j]);
    }
    printf("\n");
  } 
}

void suppr_smp()
{
    shmctl(memoire_p, IPC_RMID, NULL);
}
