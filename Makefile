
all: tmxparser.o main.o tinyxml2.o base64.o compression.o
	g++ $^ -o tmxparse_test -pthread -Wl,--no-as-needed -lz -lzstd

tmxparser.o: ./src/tmxparser.cpp ./src/base64.cpp ./src/compression.cpp ./src/tmxparser.h
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ ./src/tmxparser.cpp
	
main.o: main.cpp
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ main.cpp

tinyxml2.o: ./libs/tinyxml2/tinyxml2.cpp
	g++ -g -pthread -std=c++11 -c -I./libs/tinyxml2/ ./libs/tinyxml2/tinyxml2.cpp
	
base64.o: ./src/base64.cpp
	g++ -g -pthread -std=c++11 -c ./src/base64.cpp

compression.o: ./src/compression.cpp
	g++ -g -pthread -std=c++11 -c ./src/compression.cpp

clean:
	rm tmxparser.o main.o tinyxml2.o base64.o compression.o tmxparse_test
