
OS := $(shell uname -s)


OBJDIR = o

ifeq ($(OS),Linux)
CXX = g++
LD = g++
CXXFLAGS = -std=c++14 -g -MMD -Wno-multichar -I variant/include -I fmt
endif


ifeq ($(OS),Darwin)
CXX = clang++
LD = clang++
CXXFLAGS = -std=c++14 -g -MMD -Wno-multichar -I variant/include -I fmt
endif


SRC = main.cpp grammar.cpp parser.cpp lexer.cpp Instruction.cpp Machine.cpp OpCode.cpp \
	Expression.cpp Expression_evaluate.cpp Expression_simplify.cpp Expression_visitor.cpp \
	dp_register_set.cpp classify.cpp peephole.cpp classify.cpp basic_block.cpp \
	intern.cpp symbol_table.cpp data.cpp omf.cpp assign_pc.cpp assign_reg.cpp \
	line.cpp branch.cpp propagate_const.cpp reg_const.cpp \
	cxx/filesystem.cpp cxx/path.cpp cxx/directory_iterator.cpp cxx/mapped_file.cpp \
	register_lifetime.cpp register_set.cpp \
	opt_consolidate.cpp opt_dead_block_elimination.cpp opt_data_flow_analysis.cpp \
	opt_merge_blocks.cpp

OBJS = $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))
DEP = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.d))


asm816: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# OBJDIR is order-only prerequisite.
$(OBJS): | $(OBJDIR)

-include $(DEP)

clean:
	$(RM) grammar.cpp grammar.h lexer.cpp
	$(RM) -rf $(OBJDIR)


$(OBJDIR)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 


$(OBJDIR)/cxx/%.o : cxx/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 

%.cpp : %.rl
	ragel -G1 -L -C $^ -o $@

%.cpp : %.lemon
	lemon++ $^

# build subdirectories


$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/cxx