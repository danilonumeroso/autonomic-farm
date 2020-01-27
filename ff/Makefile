CXX	 = g++
CXXFLAGS = -std=c++17 -static-libstdc++ -O3 -I ~/fastflow
LIBS	 = -lpthread
BINDIR   = bin
TEST     = .
REPORT    = -D ENABLE_REPORT

all: test_418 test_148 test_841 constant_test

test_418: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/ff_test_418 $(LIBS) -D TEST_4L_1L_8L $(REPORT)

test_148: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/ff_test_148 $(LIBS) -D TEST_1L_4L_8L $(REPORT)

test_841: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/ff_test_841 $(LIBS) -D TEST_8L_4L_1L $(REPORT)

constant_test: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/ff_constant_test $(LIBS) -D CONSTANT_TEST $(REPORT)
