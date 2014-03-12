
all: tmx.o main.o tinyxml2.o
	g++ tmx.o main.o tinyxml2.o -o tmxparse_test
	
tmx.o: tmx.cpp
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ tmx.cpp
	
main.o: main.cpp
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ main.cpp

tinyxml2.o: ./libs/tinyxml2/tinyxml2.cpp
	g++ -g -std=c++11 -c -I./libs/tinyxml2/ ./libs/tinyxml2/tinyxml2.cpp
	
clean:
	rm tmx.o main.o tinyxml2.o tmxparse_test
