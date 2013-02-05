CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++0x -g
SF = ../src
HF = ../src/include
BF = ../bin
OB = operator.o parser.o error.o env.o object.o token.o main.o

all : $(BF)/spooner

main.o : $(SF)/main.cpp $(HF)/env.h $(HF)/error.h $(HF)/object.h $(HF)/parser.h $(HF)/token.h $(HF)/operator.h
	$(CC) $(CFLAGS) -c $(SF)/main.cpp

env.o : $(SF)/env.cpp $(HF)/env.h
	$(CC) $(CFLAGS) -c $(SF)/env.cpp

error.o : $(SF)/error.cpp $(HF)/error.h
	$(CC) $(CFLAGS) -c $(SF)/error.cpp

object.o : $(SF)/object.cpp $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/object.cpp

parser.o : $(SF)/parser.cpp $(HF)/parser.h $(HF)/operator.h $(HF)/error.h
	$(CC) $(CFLAGS) -c $(SF)/parser.cpp

token.o : $(SF)/token.cpp $(HF)/token.h
	$(CC) $(CFLAGS) -c $(SF)/token.cpp

operator.o : $(SF)/operator.cpp $(HF)/operator.h
	$(CC) $(CFLAGS) -c $(SF)/operator.cpp

$(BF)/spooner : $(OB)
	$(CC) $(OB) -o $(BF)/spooner
