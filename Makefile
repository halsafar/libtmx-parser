
all: tmx.o main.o tinyxml2.o
	g++ tmx.o main.o -o tmxparse_test
	
tmx.o: tmx.cpp
	g++ -c tmx.cpp
	
main.o: main.cpp
	g++ -c -I./libs/tinyxml2/ main.cpp

tinyxml2.o: ./libs/tinyxml2/tinyxml2.cpp
	g++ -c -I./libs/tinyxml2/ ./libs/tinyxml2/tinyxml2.cpp
	
clean:
	rm tmx.o main.o tinyxml2.o tmxparse_test
