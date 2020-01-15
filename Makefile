CXX	 = g++
CXXFLAGS = -std=c++11
LIBS	 = -lpthread
BINDIR   = bin
TEST     = test

all: 4l1l8l 1l4l8l 8l4l1l 8l1l4l

4l1l8l: $(TEST)/main.cc
	$(CXX) $(FLAGS) $^ -o $(BINDIR)/418 $(LIBS) -D TEST_4L_1L_8L

1l4l8l: $(TEST)/main.cc
	$(CXX) $(FLAGS) $^ -o $(BINDIR)/184 $(LIBS) -D TEST_1L_4L_8L

8l4l1l: $(TEST)/main.cc
	$(CXX) $(FLAGS) $^ -o $(BINDIR)/841 $(LIBS) -D TEST_8L_4L_1L

8l1l4l: $(TEST)/main.cc
	$(CXX) $(FLAGS) $^ -o $(BINDIR)/814 $(LIBS) -D TEST_8L_1L_4L
