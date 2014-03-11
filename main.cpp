
#include <iostream>

#include "tmx.h"


int main()
{
	std::cout << "Hello World" << std::endl;

	TmxParser::Tmx tmx;
	tmx.parseFromFile("example.tmx");

	return 0;
}
