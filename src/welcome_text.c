//#include "ANSI-color-codes.h"
#include <stdio.h>

#define reset "\e[0m"
#define BRED "\e[1;31m"
#define WHT "\e[0;37m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"

int main(void)
{
	printf(BRED "╔ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ╗\n");
	printf("                                             \n");
	printf("║" WHT "      WELCOME TO THE 42 POSTAL SERVICE     " BRED "║\n");
	printf("                                             \n");
	printf("╚ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ═ ╝\n\n\n" reset);
	printf(WHT"Do you want a postcard from "YEL"Lisbon"WHT" or "BLU"Seoul"WHT"?\n\n" reset);


}