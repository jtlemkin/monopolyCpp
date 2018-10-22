hoarding.out: main.o userDefinedTypes.o
	gcc -g -Wall -Werror -o hoarding.out main.o userDefinedTypes.o

main.o: main.c userDefinedTypes.h
	gcc -g -Wall -Werror -c main.c

userDefinedTypes.o:
	gcc -g -Wall -Werror -c userDefinedTypes.c

clean:
	rm -fr *.o *.out