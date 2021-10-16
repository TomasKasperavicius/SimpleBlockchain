main: User.o Header.o 
	g++ Header.o User.o main.cpp -o main -fopenmp -lcryptopp 
Header.o:
	g++ -c Header.cpp -fopenmp
User.o:
	g++ -c User.cpp
clean:
	rm *.o main *.txt