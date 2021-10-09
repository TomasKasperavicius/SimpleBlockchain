main: User.o Header.o 
	g++ -o main Header.o User.o main.cpp -I /usr/local/boost_1_77_0/include   
Header:
	g++ -c Header.cpp
User:
	g++ -c User.cpp -I /usr/local/boost_1_77_0/include
clean:
	rm *.o main