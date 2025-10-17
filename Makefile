CXX       := clang++
CXXFLAGS  := -std=c++23 -Wall -Wextra -Wno-missing-field-initializers -g
SRC_DIR   := src
TARGET    := ab
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

all: 
	${CXX} ${SRC_FILES} -o ${TARGET} 

run: all
	./${TARGET} load examples/prog.aba

run-bin: all
	./${TARGET} run examples/prog.ab

lex: all
	./${TARGET} lex examples/prog.aba

clean:
	rm -f $(TARGET) $(OBJ_FILES) $(PCH)

.PHONY: all clean run
