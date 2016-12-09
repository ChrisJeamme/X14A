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

/* Retourne aléatoirement le type de la demande d'archive */
/* 1 pour une consultation */
/* 2 pour une publication */
/* 3 pour une suppression */

typedef struct
{
  int numero;
  char article[MAX_ARTICLE][5];
}theme;
theme* liste_themes;    //La liste des thèmes

void terminaison_fils(int signal);
char* demande_archive();
void le_gros_sigaction();
char* generer_texte_aleatoire();
void initialiser_liste_themes();
void stockage_smp(char* fichier, int code);
void afficher_liste_themes();

pid_t *liste_pid;
int nb_archivistes;
int nb_themes;
int memoire_p = -1;

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
    
    /* Création liste des thèmes et partage en SMP*/

        initialiser_liste_themes();
        afficher_liste_themes();

    /* Création des archivistes*/
        liste_pid = malloc(nb_archivistes * sizeof(pid_t)); //pour stocker le pid de chacun des archivistes
        int i;
        for(i=0; i<nb_archivistes; i++) 
        {
            pid_t pid = fork();  //Création de l'archiviste
            char num_ordre[10]="";
            sprintf(num_ordre, "%d", i+1);  //Convertion du numero en chaine pour le transmettre en paramètre à arhiviste.
            
            if(pid == -1)   //Erreur
            {
                fprintf(stderr, "Erreur de fork\n");
                exit(-1);
            }
            if(pid == 0)    //Fils
            {
                //printf("Lancement du fils %d\n", pid);
                char* arguments[] = {"archiviste", num_ordre, argv[2], NULL}; //Numéro d'ordre , Nombre de thèmes
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

    /* Création des journalistes */
        while(1)
        {
            pid_t pid = fork();
            char *requete = demande_archive();
            char theme_concerne[10];
            int num_theme = rand()%nb_themes+1; //Thème aléatoire 
            sprintf(theme_concerne, "%d", num_theme);    //Convertion de num_theme en chaine pour le transmettre au journaliste en parametre

            char texte[10]="";
            if (requete[0]=='p')    //publication
            {
                char *chaine = generer_texte_aleatoire();
                strcpy(texte,chaine);     // texte de l'article
            }
            else 
            {
                int num_article = rand()%15+1;  //numéro d'article aléatoire
                sprintf(texte, "%d", num_article);  //convertion en chaine
            }
            
            if(pid == -1)   //Erreur
            {
                fprintf(stderr, "Erreur de fork\n");
                exit(-1);
            }
            if(pid == 0)    //Fils
            {
                //printf("Lancement du fils %d\n", pid);
                char* arguments[] = {"journalistes", argv[1], requete, theme_concerne, texte,NULL}; //nombre d'archivistes, requete, theme de l'article, texte ou num_article
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
    printf("Demande de terminaison des fils (signal %d)\n", signal);
    int i;
    for(i=0; i<nb_archivistes; i++)
    {
        kill(liste_pid[i], SIGKILL);    //On envoi SIGTERM à tout les archivistes
    }
    shmctl(memoire_p, IPC_RMID, NULL);
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

char* generer_texte_aleatoire()
{
    char *chaine=NULL;
    chaine=malloc(4*sizeof(char));
    int i;
    for(i=0; i<4; i++)
    {
        chaine[i]=rand()%(122-97)+97;   //lettre minuscule aleatoire.
    }
    chaine[4]='\0';
    return chaine;
}

/* Fonctions SMP */

void initialiser_liste_themes()
{
  liste_themes = NULL;
  liste_themes = (theme*) malloc(nb_themes * sizeof(theme));
  if (liste_themes == NULL)
  {
    perror("malloc");
  }
  printf("Liste thèmes créé avec succès\n");

  int i, j;
  for(i=1; i<=nb_themes; i++) //On rempli tous les thèmes
  {
    liste_themes[i].numero = i;
    
    for(j=1; j<(MAX_ARTICLE/2); j++)  //On rempli la moitié des articles de ce thème
    {
      strcpy(liste_themes[i].article[j], generer_texte_aleatoire());
      //printf("article %d\n", j);
    }
    for(j=(MAX_ARTICLE/2); j<=MAX_ARTICLE; j++)  //On rempli l'autre moitié avec VIDE
    {
      strcpy(liste_themes[i].article[j], "VIDE");
      //printf("%d article %d\n", MAX_ARTICLE, j);
    }
    
  }

  printf("Liste des thèmes rempli avec succès\n");
}

void stockage_smp(char* fichier, int code)
{
  key_t cle = ftok("initial.c", 'z');
  if (cle == -1)
  {
    perror("ftok");
    exit(EXIT_FAILURE);
  }

  char* charal;

  if((memoire_p = shmget(cle, 1024, IPC_CREAT | 0660)) != -1)
  {
    if((charal = shmat(memoire_p, 0, 0)))
    {
      *charal = 'a';

      printf("Char: %c\n", *charal);
      
      shmdt(&charal);
    }
  }
  else
  {
      perror("smhget a renvoyé -1...\n");
  }
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
