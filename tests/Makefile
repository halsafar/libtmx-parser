
all: tmxparser.o tests.o tinyxml2.o base64.o
	g++ $^ -o tmxparse_test -pthread -l gtest -Wl,--no-as-needed
	
tmxparser.o: ../src/tmxparser.cpp ../src/base64.cpp ../src/tmxparser.h
	g++ -g -pthread -std=c++11 -c -I../libs/tinyxml2/ ../src/tmxparser.cpp
	
tests.o: tests.cpp
	g++ -g -pthread -std=c++11 -c -I../libs/tinyxml2/ tests.cpp

tinyxml2.o: ../libs/tinyxml2/tinyxml2.cpp
	g++ -g -pthread -std=c++11 -c -I../libs/tinyxml2/ ../libs/tinyxml2/tinyxml2.cpp
	
base64.o: ../src/base64.cpp
	g++ -g -pthread -std=c++11 -c ../src/base64.cpp
	
clean:
	rm tmxparser.o tests.o tinyxml2.o tmxparse_test
