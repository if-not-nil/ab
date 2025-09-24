SRC_FILES = src/main.cpp
CXX_FLAGS = -Wall -Wextra -Wno-missing-field-initializers -g -std=c++23
CXX = c++

all:
	${CXX} ${SRC_FILES} ${CC_FLAGS} -o ab

run:
	make all && ./ab prog.aba

lex:
	make all && ./ab lex
