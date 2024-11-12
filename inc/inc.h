#ifndef	INC_H
# define INC_H
# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>

typedef struct s_inc
{
	char	**image_matrix;
}	t_inc;

typedef struct s_user
{
	int		index;
	char	*username;
	char	*message;
}	t_user;

#endif