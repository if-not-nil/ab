SRC_FILES = src/main.c
CC_FLAGS = -Wall -Wextra -Wno-missing-field-initializers -g -std=c11

all:
	cc ${SRC_FILES} ${CC_FLAGS} -o ab

run:
	make all && ./ab
