main: Header.o
	g++ -o main main.cpp Header.o
Header:
	g++ -c Header.cpp