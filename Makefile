SRC_FILES = src/main.c
CC_FLAGS = -Wall -Wextra -Wno-missing-field-initializers -g -std=c23

all:
	cc ${SRC_FILES} ${CC_FLAGS} -o ab

mem:
	cc src/memory.c -o mem && ./mem

run:
	make all && ./ab prog.aba

lex:
	make all && ./ab lex
