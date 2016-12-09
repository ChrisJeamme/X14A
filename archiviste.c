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
void recup_smp(int code);
int ajout_article(int numero_theme, char* article);
int suppr_article(int numero_theme, int numero_article);
void afficher_liste_themes();
void initialiser_liste_themes();
void suppr_smp();
void archiviste_sigaction();
void terminer_archiviste(int signal);

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
        //int numero_ordre = atoi(argv[1]);
        nb_themes = atoi(argv[2]);
        

    /* Récupération ensembles de mémoire partagé */
		
        initialiser_liste_themes();

		//afficher_liste_themes();

        recup_tout_smp();
        //recup_smp(2);   //Dans memoire_p

        afficher_liste_themes();

      int semaphore = 0;

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
            //printf("archiviste %d: Message bien reçu : \nrequete : %c\ntheme : %d\ntexte : %s\npid journaliste : %d\n",numero_ordre, message.requete, message.theme, message.texte, message.identite);

            if (message.requete == 'p') //publication
            {
				ajout_article(message.theme, message.texte);
            }
			else if (message.requete == 'c') //consultation
			{
				if (strcmp(liste_themes[message.theme].article[atoi(message.texte)], "VIDE")!=0)
				{
					//envoyer message au journaliste avec le contenu de l'article
				}
				else 
				{
					//envoyer message lui disant qu'il n'y a pas cet article.
				}
			}
			else // effacement
			{
				suppr_article(message.theme, atoi(message.texte));
			}
			afficher_liste_themes();
            
            
        }

    while(1);

    exit(0);
}

/* Récupère les segment de mémoire partagé pour tous les thèmes (nb_themes thèmes) */
void recup_tout_smp()
{
  int i;
  for(i=1; i<=nb_themes; i++)
  {
    recup_smp(i+1);
  }
}

/* Récupère le segement partagé pour un numéro de thème donné */
void recup_smp(int numero_theme)
{
  key_t cle_m = numero_theme;
  
  numero_theme--;
  char* article;

  if((memoire_p = shmget(cle_m, 0, 0)) != -1)
  {
    if((article = shmat(memoire_p, 0, 0)))
    {
		//Split des articles
		int j;
		for(j=1; j<=MAX_ARTICLE; j++)
		{
        	snprintf(liste_themes[numero_theme].article[j], 5, "%s", article+j*4);
			liste_themes[numero_theme].article[j][4]='\0';
			liste_themes[numero_theme].numero = numero_theme;
		}

      shmdt(&article);
    }
  }
  else
  {
      perror("smhget de recup a renvoyé -1...\n");
  }
}

/* Fonction de modification de liste_themes */
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

/* Fonction de modification de liste_themes */
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

/* Fonction de modification de liste_themes */
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

/* Affichage du tableu de structure theme */
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

/* Initialise le tableau de structure theme (vide) */
void initialiser_liste_themes()
{
	liste_themes = NULL;
	liste_themes = (theme*) malloc((nb_themes+1) * sizeof(theme));	//Allocation de la liste des thèmes
	if (liste_themes == NULL)
	{
		perror("malloc");
	}
	printf("Liste thèmes créé avec succès\n");
}

/* Supprimer le segment partagé */
void suppr_smp()
{
    shmctl(memoire_p, IPC_RMID, NULL);
}

/* Sigaction pour SIGUSR1 (signal de fermeture "propre" donné depuis initial) */
void archiviste_sigaction()
{
    struct sigaction s_terminer_archiviste;        //Création gestionnaire
    s_terminer_archiviste.sa_handler = &terminer_archiviste;   //Adresse fonction du gestionnaire
    s_terminer_archiviste.sa_flags = 0;            //Ignoré
    sigemptyset(&s_terminer_archiviste.sa_mask);   //Aucun signaux masqué => ensemble signal vide dans sa_mask

    sigaction(SIGUSR1, &s_terminer_archiviste, 0);	//Signal pour quitter "proprement"
}

/* Fonction déclenché par le signal de fermeture "propre" donné depuis initial */
void terminer_archiviste(int signal)
{
    suppr_smp();	//On supprime le segment partagé
    exit(-1);
}