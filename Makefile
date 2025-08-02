CC=gcc
NAME=ttymonit

all:
	${CC} -o ${NAME} main.c

clean:
	rm -rf ${NAME}
