

#include <iostream>

#include "tmx.h"


namespace TmxParser
{


Tmx::Tmx()
{
	// TODO Auto-generated constructor stub

}


Tmx::~Tmx()
{
	// TODO Auto-generated destructor stub
}


std::unique_ptr<TmxMap> Tmx::parseFromFile(const std::string& fileName)
{
	std::unique_ptr<TmxMap> retVal = std::unique_ptr<TmxMap>(new TmxMap());

	tinyxml2::XMLDocument doc;
	doc.LoadFile(fileName.c_str());

	std::cout << doc.FirstChildElement("map")->FloatAttribute("width") << std::endl;
	std::cout << doc.FirstChildElement("map")->Name() << std::endl;
	return retVal;
}


}
