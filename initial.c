#include <stdio.h>
#include <stdlib.h>

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

    return 0;
}