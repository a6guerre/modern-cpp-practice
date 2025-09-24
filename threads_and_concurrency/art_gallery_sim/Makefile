CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17
LDFLAGS := -lpthread
OBJ := main.o Persons.o Watchman.o PersonGenerator.o

# Default target
all: main

# Link step
main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

# Compile steps
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ) main
