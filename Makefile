build: utils.c spkmeans.c
	gcc -ansi -Wall -Wextra -Werror -pedantic-errors utils.c spkmeans.c -o spkmeans -lm
