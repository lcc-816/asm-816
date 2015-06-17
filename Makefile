
OS := $(shell uname -s)


ifeq ($(OS),Linux)
CXX = g++
CXXFLAGS = -std=c++11 -g -MMD -Wno-multichar
endif


ifeq ($(OS),Darwin)
CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -g -MMD -Wno-multichar
endif


SRC = grammar.cpp main.cpp lexer.cpp Instruction.cpp Machine.cpp OpCode.cpp \
	Expression.cpp Expression_evaluate.cpp \
	register_set.cpp classify.cpp peephole.cpp classify.cpp basic_block.cpp \
	intern.cpp symbol_table.cpp data.cpp omf.cpp set_pc.cpp

OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

asm816: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEP)

clean:
	$(RM) $(OBJ) $(DEP) grammar.cpp grammar.h lexer.cpp

lexer.cpp: lexer.rl
	ragel -G1 -L -C $^ -o $@

grammar.cpp: grammar.lemon
	lemon $^ 
	mv grammar.c grammar.cpp
