

CXX = clang++ -std=c++14
CXXFLAGS = -I..

TESTS = ut_dp_register # ut_branch
LIBS =  dp_register_set.o  \
	Expression.o Expression_evaluate.o Expression_simplify.o Expression_visitor.o

unit_test: ut_main.o $(patsubst %,%.o,$(TESTS)) $(patsubst %,../o/%,$(LIBS))
	$(CXX) -o $@ $^ 

ut_main.o: ut_main.cpp catch.hpp

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 