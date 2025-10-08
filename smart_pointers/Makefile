# Simple Makefile for main.cpp with C++20 and pthread

CXX := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++20
LDFLAGS := -lpthread

TARGET := main
SRC := main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
