

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
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		printf("Cannot read xml file\n");
		return nullptr;
	}

	std::unique_ptr<TmxMap> retVal = _parseMapNode(doc.FirstChildElement("map"));

	return retVal;
}


std::unique_ptr<TmxMap> Tmx::_parseMapNode(tinyxml2::XMLElement* element)
{
	std::unique_ptr<TmxMap> retVal = std::unique_ptr<TmxMap>(new TmxMap());

	if (element == NULL)
	{
		return nullptr;
	}

	retVal->version = element->Attribute("version");
	retVal->orientation = element->Attribute("orientation");
	retVal->width = element->UnsignedAttribute("width");
	retVal->width = element->UnsignedAttribute("height");
	retVal->tileWidth = element->UnsignedAttribute("tilewidth");
	retVal->tileHeight = element->UnsignedAttribute("tileheight");
	retVal->backgroundColor = element->Attribute("backgroundcolor");
	retVal->propertyMap = this->_parsePropertyNode(element->FirstChildElement("properties"));

	std::cout << element->FloatAttribute("width") << std::endl;
	std::cout << element->Name() << std::endl;

	return retVal;
}


TmxPropertyMap_t Tmx::_parsePropertyNode(tinyxml2::XMLElement* element)
{
	TmxPropertyMap_t retVal;

	tinyxml2::XMLElement* propertyElement = element->FirstChildElement("property");

	for (tinyxml2::XMLElement* child = propertyElement->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		if (strcmp(child->Name(), "property"))
		{
			//element->Attribute("name")
		}
	}


	return retVal;
}


}
