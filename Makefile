CXX       := clang++
CXXFLAGS  := -std=c++23 -Wall -Wextra -Wno-missing-field-initializers -g
SRC_DIR   := src
TARGET    := ab
PCH       := $(SRC_DIR)/prelude.pch
PCH_HEADER:= $(SRC_DIR)/prelude.hpp
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(PCH) $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) $(CXXFLAGS) -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(PCH)
	$(CXX) $(CXXFLAGS) -include-pch $(PCH) -c $< -o $@

$(PCH): $(PCH_HEADER)
	$(CXX) $(CXXFLAGS) -x c++-header $(PCH_HEADER) -o $(PCH)

run: all
	./${TARGET} load prog.aba

run-bin: all
	./${TARGET} run prog.ab

lex: all
	./${TARGET} lex prog.aba

clean:
	rm -f $(TARGET) $(OBJ_FILES) $(PCH)

.PHONY: all clean run
