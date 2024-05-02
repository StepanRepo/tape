SHELL := /bin/bash
.PHONY: run del


# compiller settings
CXX = g++

errors = -Wall -Wextra -pedantic #-Werror 
mem = -fbounds-check -g
opt = -O2 # -fopenmp


# helpful variables
msg = lib/massages.h

files = $(wildcard src/*.cpp)
obj = $(files:src/%.cpp=.o/%.o) .o/main.o

compile_current = $(CXX) $(errors) $(mem) $(opt) -c $< -o $@




main: $(msg) .o/configuration.o .o/tape.o .o/main.o
	$(CXX) $(errors) $(mem) $(opt) $^ -o $@

# Compile object files

.o/%.o: src/%.cpp lib/%.h $(msg)
	$(compile_current)

.o/%.o: src/%.cpp $(msg)
	$(compile_current)

# Some run commands
run: main
	clear
	./main

del: 
	rm .o/* main
