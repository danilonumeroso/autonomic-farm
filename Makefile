CXX	 = g++
CXXFLAGS = -std=c++11
LIBS	 = -lpthread
BINDIR   = bin
TEST     = test

all: main

main: $(TEST)/main.cc
	$(CXX) $(FLAGS) $^ -o $(BINDIR)/main $(LIBS)
