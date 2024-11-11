#include "../inc/inc.h"

int	main()
{
	char	*inp = NULL;
	size_t	len = 0;
	int	ret_value;
	printf("Welcome to our project!\n");
	while (1)
	{
		ret_value = getline(&inp, &len, stdin);
		if (ret_value == -1)
			break;
		else if (!strcmp(inp, "1\n"))
			printf("works\n");
		else
			printf("doesnt work\n");
	}
	free(inp);
	return 0;
}