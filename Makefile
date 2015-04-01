CXX = clang++
CXXFLAGS = -std=c++11 -stdlib=libc++ -g
OBJECTS = main.o lexer.o grammar.o Expression.o Instruction.o Machine.o

asm816: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(OBJECTS) grammar.cpp lexer.cpp

lexer.cpp: lexer.rl
	ragel -G1 -L -C $^ -o $@

grammar.cpp : grammar.lemon
	lemon $^ 
	mv grammar.c grammar.cpp