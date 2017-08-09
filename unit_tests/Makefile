

CXX = clang++
CXXFLAGS = -I.. -g -std=c++14

TESTS = ut_dp_register ut_scoped_map # ut_branch
LIBS =  dp_register_set.o  \
	Expression.o Expression_evaluate.o Expression_simplify.o Expression_visitor.o

unit_test: ut_main.o $(patsubst %,%.o,$(TESTS)) $(patsubst %,../o/%,$(LIBS))
	$(CXX) -o $@ $^ 

ut_main.o: ut_main.cpp catch.hpp

ut_scoped_map.o : ut_scoped_map.cpp ../include/scoped_map.h

%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $< 