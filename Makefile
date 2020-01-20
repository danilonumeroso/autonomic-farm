CXX	 = g++
65;5602;1cCXXFLAGS = -Wall -Wextra -std=c++11 -static-libstdc++
LIBS	 = -lpthread
BINDIR   = bin
TEST     = test
REPORT    = -D ENABLE_REPORT

all: tiny_418 tiny_148 tiny_841 tiny_814 large_418 large_148 large_841 large_814

tiny_418: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/tiny_418 $(LIBS) -D TINY_TEST_4L_1L_8L $(REPORT)

tiny_148: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/tiny_148 $(LIBS) -D TINY_TEST_1L_4L_8L $(REPORT)

tiny_841: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/tiny_841 $(LIBS) -D TINY_TEST_8L_4L_1L $(REPORT)

tiny_814: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/tiny_814 $(LIBS) -D TINY_TEST_8L_1L_4L $(REPORT)

large_418: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/large_418 $(LIBS) -D LARGE_CONSTANT_TEST_4L_1L_8L -D LARGE_TEST $(REPORT)

large_148: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/large_148 $(LIBS) -D LARGE_CONSTANT_TEST_1L_4L_8L -D LARGE_TEST $(REPORT)

large_841: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/large_841 $(LIBS) -D LARGE_CONSTANT_TEST_8L_4L_1L -D LARGE_TEST $(REPORT)

large_814: $(TEST)/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/large_814 $(LIBS) -D LARGE_CONSTANT_TEST_8L_1L_4L -D LARGE_TEST $(REPORT)
