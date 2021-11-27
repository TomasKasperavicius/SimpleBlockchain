main: User.o Header.o 
	g++ Header.o User.o main.cpp -o main -fopenmp -lcryptopp $(shell pkg-config --cflags --libs libbitcoin-system)
Header.o:
	g++ -c Header.cpp -fopenmp $(shell pkg-config --cflags --libs libbitcoin-system)
User.o:
	g++ -c User.cpp
clean:
	rm *.o main *.txt