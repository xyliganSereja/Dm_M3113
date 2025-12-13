CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = hamming
SRC = hamming.cpp

TESTBIN = tests
TESTSRC = tests.cpp

ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXE = .exe
else
    RM = rm -f
    EXE =
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET)$(EXE) $(SRC)

$(TESTBIN): $(TESTSRC)
	$(CXX) $(CXXFLAGS) -o $(TESTBIN)$(EXE) $(TESTSRC)

test: $(TARGET) $(TESTBIN)
	./$(TESTBIN)$(EXE)

clean:
	$(RM) $(TARGET)$(EXE) $(TESTBIN)$(EXE)
