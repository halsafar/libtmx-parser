
all: tmxparser.o main.o tinyxml2.o
	g++ tmxparser.o main.o tinyxml2.o -o tmxparse_test -pthread -Wl,--no-as-needed
	
tmxparser.o: ./src/tmxparser.cpp ./src/tmxparser.h
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ ./src/tmxparser.cpp
	
main.o: main.cpp
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ main.cpp

tinyxml2.o: ./libs/tinyxml2/tinyxml2.cpp
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ ./libs/tinyxml2/tinyxml2.cpp
	
clean:
	rm tmxparser.o main.o tinyxml2.o tmxparse_test
