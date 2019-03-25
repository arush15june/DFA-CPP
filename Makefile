CPP_STD=14
CPP_FLAGS=-std=c++$(CPP_STD) -ggdb
SOURCE=dfa.cpp
OUTPUT=dfa_bin

block:
	g++ $(CPP_FLAGS) -o $(OUTPUT) $(SOURCE)