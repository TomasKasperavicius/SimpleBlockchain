main: User.o Header.o 
	g++ -o main Header.o User.o main.cpp -lcryptopp 
Header: User.o
	g++ -c Header.cpp
User:
	g++ -c User.cpp
clean:
	rm *.o main *.txt