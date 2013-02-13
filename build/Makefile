CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++0x -g
SF = ../src
HF = ../src/include
BF = ../bin
OB = object.o value.o name.o refobject.o gcobject.o list.o function.o
AL = operator.o parser.o error.o env.o $(OB) token.o vm.o pattern.o repl.o main.o

all : $(BF)/spooner

main.o : $(SF)/main.cpp $(HF)/env.h $(HF)/error.h $(HF)/object.h $(HF)/parser.h $(HF)/token.h $(HF)/operator.h $(HF)/vm.h
	$(CC) $(CFLAGS) -c $(SF)/main.cpp

env.o : $(SF)/env.cpp $(HF)/env.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/env.cpp

error.o : $(SF)/error.cpp $(HF)/error.h
	$(CC) $(CFLAGS) -c $(SF)/error.cpp

object.o : $(SF)/object.cpp $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/object.cpp

value.o : $(SF)/value.cpp $(HF)/value.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/value.cpp

refobject.o : $(SF)/refobject.cpp $(HF)/refobject.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/refobject.cpp

gcobject.o : $(SF)/gcobject.cpp $(HF)/gcobject.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/gcobject.cpp

name.o : $(SF)/name.cpp $(HF)/name.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/name.cpp

list.o : $(SF)/list.cpp $(HF)/list.h $(HF)/gcobject.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/list.cpp

function.o : $(SF)/function.cpp $(HF)/function.h $(HF)/env.h $(HF)/list.h $(HF)/gcobject.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/function.cpp

parser.o : $(SF)/parser.cpp $(HF)/parser.h $(HF)/operator.h $(HF)/error.h
	$(CC) $(CFLAGS) -c $(SF)/parser.cpp

token.o : $(SF)/token.cpp $(HF)/token.h
	$(CC) $(CFLAGS) -c $(SF)/token.cpp

operator.o : $(SF)/operator.cpp $(HF)/operator.h
	$(CC) $(CFLAGS) -c $(SF)/operator.cpp

vm.o : $(SF)/vm.cpp $(HF)/object.h $(HF)/env.h $(HF)/parser.h $(HF)/error.h $(HF)/operator.h $(HF)/repl.h
	$(CC) $(CFLAGS) -c $(SF)/vm.cpp

pattern.o : $(SF)/pattern.cpp $(HF)/pattern.h $(HF)/object.h
	$(CC) $(CFLAGS) -c $(SF)/pattern.cpp

repl.o : $(SF)/repl.cpp $(HF)/repl.h $(HF)/error.h
	$(CC) $(CFLAGS) -c $(SF)/repl.cpp

$(BF)/spooner : $(AL)
	$(CC) $(AL) -o $(BF)/spooner
