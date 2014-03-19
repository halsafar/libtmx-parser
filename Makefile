
all: tmxparser.o main.o tinyxml2.o
	g++ tmxparser.o main.o tinyxml2.o -o tmxparse_test
	
tmxparser.o: tmxparser.cpp tmxparser.h
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ tmxparser.cpp
	
main.o: main.cpp
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ main.cpp

tinyxml2.o: ./libs/tinyxml2/tinyxml2.cpp
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ ./libs/tinyxml2/tinyxml2.cpp
	
clean:
	rm tmxparser.o main.o tinyxml2.o tmxparse_test
