
OS := $(shell uname -s)


OBJDIR = o

ifeq ($(OS),Linux)
CXX = g++
CXXFLAGS = -std=c++14 -g -MMD -Wno-multichar
endif


ifeq ($(OS),Darwin)
CXX = clang++
CXXFLAGS = -std=c++14 -stdlib=libc++ -g -MMD -Wno-multichar
endif


SRC = grammar.cpp main.cpp lexer.cpp Instruction.cpp Machine.cpp OpCode.cpp \
	Expression.cpp Expression_evaluate.cpp Expression_simplify.cpp Expression_visitor.cpp \
	register_set.cpp classify.cpp peephole.cpp classify.cpp basic_block.cpp \
	intern.cpp symbol_table.cpp data.cpp omf.cpp assign_pc.cpp assign_reg.cpp \
	line.cpp branch.cpp propagate_const.cpp reg_const.cpp \
	cxx/filesystem.cpp cxx/path.cpp cxx/directory_iterator.cpp \
	common_line_consolidation.cpp register_lifetime.cpp register_bits.cpp

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
DEP = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.d))


asm816: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# OBJDIR is order-only prerequisite.
$(OBJS): | $(OBJDIR)

-include $(DEP)

clean:
	$(RM) grammar.cpp grammar.h lexer.cpp
	$(RM) -rf $(OBJDIR)

lexer.cpp: lexer.rl
	ragel -G1 -L -C $^ -o $@

grammar.cpp: grammar.lemon
	lemon++ $^ 

$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 


$(OBJDIR)/cxx/%.o : cxx/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 


# build subdirectories


$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/cxx