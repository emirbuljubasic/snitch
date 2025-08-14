CC = gcc
CFLAGS =-Wall -Wextra -std=c11 -Iinclude

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

BIN = snitch
BUILD_DIR = build/bin
TARGET = ${BUILD_DIR}/${BIN}

all: ${TARGET}

${TARGET}:${OBJ}
	@mkdir -p ${BUILD_BIN}
	${CC} ${CFLAGS} -o $@ $^

clean:
	rm -rf ${OBJ} ${TARGET}

.PHONY: all clean
