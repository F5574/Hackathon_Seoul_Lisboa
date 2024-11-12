#ifndef	INC_H
# define INC_H
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <ctype.h>

//matrix
typedef struct s_inc
{
    char **image_matrix;
    char **message_matrix;
    size_t line_count;
} t_inc;

//colors
#define reset "\e[0m"
#define BRED "\e[1;31m"
#define WHT "\e[0;37m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_RESET   "\x1b[0m"

#endif