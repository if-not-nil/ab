SRC_FILES = src/main.cpp
CXX_FLAGS = -Wall -Wextra -Wno-missing-field-initializers -g -std=c++23
CXX = c++

TARGET = ab

all:
	${CXX} ${SRC_FILES} ${CXX_FLAGS} -o ${TARGET}

run:
	@make all
	./${TARGET} load prog.aba

run-bin:
	@make all
	./${TARGET} run prog.ab

lex:
	@make all
	./${TARGET} lex prog.aba

clean:
	rm -f ${TARGET} *.o
