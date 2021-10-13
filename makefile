main: User.o Header.o 
	g++ -o main Header.o User.o main.cpp -l:libcryptopp.a   
Header: User.o
	g++ -c Header.cpp
User:
	g++ -c User.cpp
clean:
	rm *.o main *.txt