
#include "tmx.h"


#if (defined(_WIN32))
     #include <string.h>
#endif

#if (( defined(ANDROID)))
     #include <android/log.h>
     #include <string.h>
#else
	#include <cstdlib>
	#include <cstdio>
#endif


#define LOG_TAG "libtmxparser"


// quote define define :D
#  define QUOTEME_(x) #x
#  define QUOTEME(x) QUOTEME_(x)


// print helpers
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__


// LOGGING
#if (( defined(ANDROID)))
     #ifdef DEBUG
          #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, " (" __FILE__ ":" QUOTEME(__LINE__) ") " __VA_ARGS__ )
     #else
          #define LOGD(...)
     #endif

     #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "" __VA_ARGS__ )
     #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )
	 #define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__ )
#else
     #ifdef DEBUG
     #define LOGD(...) fprintf(stderr, "D/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__ ); fprintf(stderr, "\n");
     #else
     #define LOGD(...)
     #endif
     #define LOGI(...) fprintf(stdout, "I/" QUOTEME(LOG_TAG) "(" ")" __VA_ARGS__ ); fprintf(stdout, "\n");
     #define LOGE(...) fprintf(stderr, "E/" QUOTEME(LOG_TAG) "(" ")" __VA_ARGS__ ); fprintf(stderr, "\n");
	 #define LOGW(...) fprintf(stderr, "W/" QUOTEME(LOG_TAG) "(" ")" __VA_ARGS__ ); fprintf(stderr, "\n");
#endif


// TODO - remove these
#define VALIDATE_ELEMENT_NAME(NAME, VERIFYNAME) \
	strcmp(NAME, VERIFYNAME) == 0 ? true : false


#define CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(XMLELEMENT, ATTRIBNAME, LHS) \
	if (XMLELEMENT->Attribute(ATTRIBNAME) != NULL) \
	{ \
		LHS = XMLELEMENT->Attribute(ATTRIBNAME); \
	} \
	else \
	{ \
		LOGE("MISSING ATTRIBUTE"); \
	}


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
		LOGE("Cannot read xml file\n");
		return nullptr;
	}

	// parse the map node
	std::unique_ptr<TmxMap> retVal = _parseMapNode(doc.FirstChildElement("map"));
	if (retVal == nullptr)
	{
		LOGE("Cannot parse the node");
		return nullptr;
	}

	// attempt to parse tileset nodes


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
	retVal->height = element->UnsignedAttribute("height");
	retVal->tileWidth = element->UnsignedAttribute("tilewidth");
	retVal->tileHeight = element->UnsignedAttribute("tileheight");
	retVal->backgroundColor = element->Attribute("backgroundcolor");

	retVal->propertyMap = this->_parsePropertyNode(element->FirstChildElement("properties"));

	for (tinyxml2::XMLElement* child = element->FirstChildElement("tileset"); child != NULL; child = child->NextSiblingElement("tileset"))
	{
		TmxTileset set = this->_parseTilesetNode(child);
		retVal->tilesetCollection.push_back(set);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("layer"); child != NULL; child = child->NextSiblingElement("layer"))
	{
		LOGI("NAME: %s", child->Name());
		retVal->layerCollection.push_back(this->_parseLayerNode(child));
	}

	return retVal;
}


TmxPropertyMap_t Tmx::_parsePropertyNode(tinyxml2::XMLElement* element)
{
	TmxPropertyMap_t retVal;

	if (element == NULL)
	{
		return retVal;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("property"); child != NULL; child = child->NextSiblingElement("property"))
	{
		if (VALIDATE_ELEMENT_NAME(child->Name(), "property"))
		{
			retVal[child->Attribute("name")] = child->Attribute("value");
		}
	}

	return retVal;
}


TmxImage Tmx::_parseImageNode(tinyxml2::XMLElement* element)
{
	TmxImage retVal;

	CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(element, "source", retVal.source);

	if (element->Attribute("format") != NULL)
	{
		retVal.format = element->Attribute("format");
	}

	CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(element, "trans", retVal.transparentColor);

	//retVal.transparentColor = element->Attribute("trans");
	retVal.width = element->UnsignedAttribute("width");
	retVal.height = element->UnsignedAttribute("height");

	return retVal;
}


TmxTileset Tmx::_parseTilesetNode(tinyxml2::XMLElement* element)
{
	TmxTileset retVal;

	if (VALIDATE_ELEMENT_NAME(element->Name(), "tileset"))
	{
		retVal.firstgid = element->UnsignedAttribute("firstgid");
		retVal.name = element->Attribute("name");
		retVal.tileWidth = element->UnsignedAttribute("tilewidth");
		retVal.tileHeight = element->UnsignedAttribute("tileheight");
		retVal.tileSpacingInImage = element->UnsignedAttribute("spacing");
		retVal.tileMarginInImage = element->UnsignedAttribute("margin");
		retVal.image = _parseImageNode(element->FirstChildElement("image"));

		for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
		{
			retVal._tiles.push_back(this->_parseTileDefinitionNode(child));
		}
	}

	return retVal;
}


TmxTileDefinition Tmx::_parseTileDefinitionNode(tinyxml2::XMLElement* element)
{
	TmxTileDefinition retVal;

	retVal.id = element->UnsignedAttribute("id");
	retVal.propertyMap = _parsePropertyNode(element->FirstChildElement("properties"));

	return retVal;
}


TmxLayer Tmx::_parseLayerNode(tinyxml2::XMLElement* element)
{
	TmxLayer retVal;

	retVal.name = element->Attribute("name");
	retVal.opacity = element->FloatAttribute("opacity");
	retVal.visible = element->IntAttribute("visible");
	retVal.width = element->UnsignedAttribute("width");
	retVal.height = element->UnsignedAttribute("height");
	retVal.propertyMap = _parsePropertyNode(element->FirstChildElement("properties"));

	// check data node and type
	tinyxml2::XMLElement* dataElement = element->FirstChildElement("data");
	if (dataElement != NULL)
	{
		retVal.tiles = _parseLayerXmlDataNode(dataElement);
	}

	return retVal;
}


TmxLayerTileCollection_t Tmx::_parseLayerXmlDataNode(tinyxml2::XMLElement* element)
{
	TmxLayerTileCollection_t retVal;
	for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
	{
		retVal.push_back(_parseLayerXmlTileNode(child));
	}
	return retVal;
}



TmxLayerTile Tmx::_parseLayerXmlTileNode(tinyxml2::XMLElement* element)
{
	TmxLayerTile retVal;

	retVal.gid = element->UnsignedAttribute("gid");

	return retVal;
}


}
