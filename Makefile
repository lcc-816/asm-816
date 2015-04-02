CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -g -MMD

SRC = main.cpp lexer.cpp grammar.cpp Expression.cpp Instruction.cpp Machine.cpp
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

asm816: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEP)

clean:
	$(RM) $(OBJ) $(DEP) grammar.cpp lexer.cpp

lexer.cpp: lexer.rl
	ragel -G1 -L -C $^ -o $@

grammar.cpp : grammar.lemon
	lemon $^ 
	mv grammar.c grammar.cpp
