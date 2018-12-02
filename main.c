#include "USBibuddy.h"
#include <stdlib.h>

int main (int argc,char **argv)

{
unsigned char state_ebuddy;

/*
//  coeur ! couleur corps !  Ailes  ! corps
//     X     x    x    x     Y   Y    Z  Z
// attention ne pas alimenter Ailes 0 et O et corps  0 0 sous peine de destruction */
#define  ebuddy_coeur           0x80
#define  ebuddy_bleu            0x40
#define  ebuddy_vert            0x20
#define  ebuddy_rouge           0x10
#define  ebuddy_corps_droite    0x02
#define  ebuddy_corps_gauche    0x01
#define  ebuddy_ailes_hautes    0x08
#define  ebuddy_ailes_basses    0x04


if (argc==2)
    {
    state_ebuddy = (unsigned char)atoi(argv [1]);
    ebuddy_envoi(state_ebuddy);
    return EXIT_SUCCESS;
    }
else
    {
    printf ("(c)2028 Bogt\n");
    printf ("syntaxe:\n");
    printf (" ./ebuddy n\n");
    printf ("n etant défini comme ceci:\n");
    printf ("ebuddy_coeur           0x80 bit 7\n");
    printf ("ebuddy_bleu            0x40 bit 6\n");
    printf ("ebuddy_vert            0x20 bit 5\n");
    printf ("ebuddy_rouge           0x10 bit 4\n");
    printf ("ebuddy_corps_droite    0x02 bit 1\n");
    printf ("ebuddy_corps_gauche    0x01 bit 0\n");
    printf ("ebuddy_ailes_hautes    0x08 bit 3\n");
    printf ("ebuddy_ailes_basses    0x04 bit 2\n");
    printf ("0<n<255\n");
    printf (" la restriction est de ne pas mettre les ailes hautes et basses en meme temps\n");
    printf ("même contrainte sur le corps \n");
    return EXIT_FAILURE;
}
}
