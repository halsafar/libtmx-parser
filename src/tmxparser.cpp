
#include "tmxparser.h"


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

#include <string>
#include <sstream>


#ifndef LOG_TAG
#define LOG_TAG "libtmxparser"
#endif

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


#define CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(XMLELEMENT, ATTRIBNAME, LHS) \
	if (XMLELEMENT->Attribute(ATTRIBNAME) != NULL) \
	{ \
		LHS = XMLELEMENT->Attribute(ATTRIBNAME); \
	} \
	else \
	{ \
		LOGW("Missing Attribute [%s]", ATTRIBNAME); \
	}


namespace tmxparser
{


// Prototypes
TmxReturn _parseMapNode(tinyxml2::XMLElement* element, TmxMap* outMap);
TmxReturn _parsePropertyNode(tinyxml2::XMLElement* element, TmxPropertyMap_t* outPropertyMap);
TmxReturn _parseImageNode(tinyxml2::XMLElement* element, TmxImage* outImage);
TmxReturn _parseTilesetNode(tinyxml2::XMLElement* element, TmxTileset* outTileset);
TmxReturn _parseTileDefinitionNode(tinyxml2::XMLElement* element, TmxTileDefinition* outTileDefinition);
TmxReturn _parseLayerNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayer* outLayer);
TmxReturn _parseLayerXmlDataNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTileCollection_t* outTileCollection);
TmxReturn _parseLayerXmlTileNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile);
//TmxLayerTileCollection_t _parseLayerCsvDataNode(tinyxml2::XMLElement* element);
//TmxLayerTileCollection_t _parseLayerBase64DataNode(tinyxml2::XMLElement* element);
TmxReturn _parseObjectGroupNode(tinyxml2::XMLElement* element, TmxObjectGroup* outObjectGroup);
TmxReturn _parseObjectNode(tinyxml2::XMLElement* element, TmxObject* outObj);



TmxReturn parseFromFile(const std::string& fileName, TmxMap* outMap)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		LOGE("Cannot read xml file");
		return TmxReturn::kErrorParsing;
	}

	// parse the map node
	return _parseMapNode(doc.FirstChildElement("map"), outMap);
}


TmxReturn parseFromMemory(void* data, size_t length, TmxMap* outMap)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse((char*)data, length))
	{
		LOGE("Cannot parse xml memory file...");
		return TmxReturn::kErrorParsing;
	}

	return _parseMapNode(doc.FirstChildElement("map"), outMap);
}


TmxReturn _parseMapNode(tinyxml2::XMLElement* element, TmxMap* outMap)
{
	if (element == NULL)
	{
		return TmxReturn::kMissingMapNode;
	}

	outMap->version = element->Attribute("version");
	outMap->orientation = element->Attribute("orientation");
	outMap->width = element->UnsignedAttribute("width");
	outMap->height = element->UnsignedAttribute("height");
	outMap->tileWidth = element->UnsignedAttribute("tilewidth");
	outMap->tileHeight = element->UnsignedAttribute("tileheight");
	outMap->backgroundColor = element->Attribute("backgroundcolor");

	TmxReturn error = _parsePropertyNode(element->FirstChildElement("properties"), &outMap->propertyMap);
	if (error)
	{
		LOGE("Error processing map properties...");
		return error;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("tileset"); child != NULL; child = child->NextSiblingElement("tileset"))
	{
		// TODO - pointer it all up?
		TmxTileset set;
		error = _parseTilesetNode(child, &set);
		if (error)
		{
			LOGE("Error processing tileset node...");
			return error;
		}

		outMap->tilesetCollection.push_back(set);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("layer"); child != NULL; child = child->NextSiblingElement("layer"))
	{
		TmxLayer layer;
		error = _parseLayerNode(child, outMap->tilesetCollection, &layer);
		if (error)
		{
			LOGE("Error processing layer node...");
			return error;
		}

		outMap->layerCollection.push_back(layer);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("objectgroup"); child != NULL; child = child->NextSiblingElement("objectgroup"))
	{
		TmxObjectGroup group;
		error = _parseObjectGroupNode(child, &group);
		if (error)
		{
			LOGE("Error processing layer node...");
			return error;
		}

		outMap->objectGroupCollection.push_back(group);
	}

	return error;
}


TmxReturn _parsePropertyNode(tinyxml2::XMLElement* element, TmxPropertyMap_t* outPropertyMap)
{
	//LOGD("_parsePropertyNode(%p, %p)", element, outPropertyMap);
	if (element == NULL)
	{
		// ignore this, not everything requires properties

		return TmxReturn::kSuccess;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("property"); child != NULL; child = child->NextSiblingElement("property"))
	{
		if (strcmp(child->Name(), "property") == 0)
		{
			LOGD("NAME:%s", child->Attribute("name"));
			LOGD("VALUE:%s", child->Attribute("value"));
			if (child->Attribute("name") != NULL && child->Attribute("value") != NULL)
			{
				(*outPropertyMap)[child->Attribute("name")] = child->Attribute("value");
			}
			else
			{
				return TmxReturn::kMalformedPropertyNode;
			}
		}
	}

	return TmxReturn::kSuccess;
}


TmxReturn _parseImageNode(tinyxml2::XMLElement* element, TmxImage* outImage)
{
	CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(element, "source", outImage->source);

	if (element->Attribute("format") != NULL)
	{
		outImage->format = element->Attribute("format");
	}

	CHECK_AND_RETRIEVE_ATTRIBUTE_STRING(element, "trans", outImage->transparentColor);

	//retVal.transparentColor = element->Attribute("trans");
	outImage->width = element->UnsignedAttribute("width");
	outImage->height = element->UnsignedAttribute("height");

	return TmxReturn::kSuccess;
}


TmxReturn _parseTilesetNode(tinyxml2::XMLElement* element, TmxTileset* outTileset)
{

	if (strcmp(element->Name(), "tileset") == 0)
	{
		outTileset->firstgid = element->UnsignedAttribute("firstgid");
		outTileset->name = element->Attribute("name");
		outTileset->tileWidth = element->UnsignedAttribute("tilewidth");
		outTileset->tileHeight = element->UnsignedAttribute("tileheight");
		outTileset->tileSpacingInImage = element->UnsignedAttribute("spacing");
		outTileset->tileMarginInImage = element->UnsignedAttribute("margin");

		TmxImage image;
		TmxReturn error = _parseImageNode(element->FirstChildElement("image"), &outTileset->image);
		if (error)
		{
			LOGE("Error parsing image node...");
			return error;
		}

		for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
		{
			TmxTileDefinition tileDef;
			error = _parseTileDefinitionNode(child, &tileDef);
			if (error)
			{
				LOGE("Error parsing tile definition");
				return error;
			}

			outTileset->_tiles.push_back(tileDef);
		}
	}

	return TmxReturn::kSuccess;
}


TmxReturn _parseTileDefinitionNode(tinyxml2::XMLElement* element, TmxTileDefinition* outTileDefinition)
{
	TmxReturn error = TmxReturn::kSuccess;

	outTileDefinition->id = element->UnsignedAttribute("id");
	error = _parsePropertyNode(element->FirstChildElement("properties"), &outTileDefinition->propertyMap);

	return error;
}


TmxReturn _parseLayerNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayer* outLayer)
{
	TmxReturn error = TmxReturn::kSuccess;

	outLayer->name = element->Attribute("name");
	outLayer->opacity = element->FloatAttribute("opacity");
	outLayer->visible = element->IntAttribute("visible");
	outLayer->width = element->UnsignedAttribute("width");
	outLayer->height = element->UnsignedAttribute("height");

	error = _parsePropertyNode(element->FirstChildElement("properties"), &outLayer->propertyMap);
	if (error)
	{
		LOGE("Error parsing layer property node...");
		return error;
	}

	// check data node and type
	tinyxml2::XMLElement* dataElement = element->FirstChildElement("data");
	if (dataElement != NULL)
	{
		error = _parseLayerXmlDataNode(dataElement, tilesets, &outLayer->tiles);
	}
	else
	{
		LOGE("Layer missing data node...");
		return TmxReturn::kMissingDataNode;
	}

	return error;
}


TmxReturn _parseLayerXmlDataNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTileCollection_t* outTileCollection)
{
	TmxReturn error = TmxReturn::kSuccess;
	for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
	{
		TmxLayerTile tile;
		error = _parseLayerXmlTileNode(child, tilesets, &tile);
		outTileCollection->push_back(tile);
	}
	return error;
}



TmxReturn _parseLayerXmlTileNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile)
{
	TmxReturn error = TmxReturn::kSuccess;

	outTile->gid = element->UnsignedAttribute("gid");

	outTile->tilesetIndex = 0;
	outTile->tileInTilesetIndex = 0;

	// search for layerindex
	// O(n) where n = number of tilesets
	// Generally n will never be high but this method is called from an O(m) method.
	unsigned int index = 0;
	//unsigned int lastStartIndex = 0;
	unsigned int lastEndIndex = 1;
	for (auto it = tilesets.begin(); it != tilesets.end(); ++it)
	{
		unsigned int startIndex = it->firstgid;
		unsigned int endIndex = it->firstgid + ( (it->image.width / it->tileWidth) * (it->image.height / it->tileHeight) );

		if (outTile->gid >= startIndex && outTile->gid < endIndex)
		{
			outTile->tilesetIndex = index;

			outTile->tileInTilesetIndex = (outTile->gid) - lastEndIndex;

			return error;
		}

		//lastStartIndex = startIndex;
		lastEndIndex = endIndex;

		index++;
	}

	return error;
}


TmxReturn _parseObjectGroupNode(tinyxml2::XMLElement* element, TmxObjectGroup* outObjectGroup)
{
	TmxReturn error = TmxReturn::kSuccess;

	outObjectGroup->name = element->Attribute("name");

	if (element->Attribute("opacity") != NULL)
	{
		outObjectGroup->opacity = element->FloatAttribute("opacity");
	}
	else
	{
		outObjectGroup->opacity = 1.0f;
	}

	if (element->Attribute("visible") != NULL)
	{
		outObjectGroup->visible = element->BoolAttribute("visible");
	}
	else
	{
		outObjectGroup->visible = 1;
	}

	error = _parsePropertyNode(element->FirstChildElement("properties"), &outObjectGroup->propertyMap);
	if (error)
	{
		return error;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("object"); child != NULL; child = child->NextSiblingElement("object"))
	{
		TmxObject obj;
		error = _parseObjectNode(child, &obj);
		if (error)
		{
			LOGE("Error parsing object node...");
			return TmxReturn::kErrorParsing;
		}
		outObjectGroup->objects.push_back(obj);
	}

	return error;
}


TmxReturn _parseObjectNode(tinyxml2::XMLElement* element, TmxObject* outObj)
{
	TmxReturn error = TmxReturn::kSuccess;

	if (element->Attribute("name"))
	{
		outObj->name = element->Attribute("name");
	}

	if (element->Attribute("type"))
	{
		outObj->type = element->Attribute("type");
	}
	outObj->x = element->IntAttribute("x");
	outObj->y = element->IntAttribute("y");
	outObj->width = element->IntAttribute("width");
	outObj->height = element->IntAttribute("height");
	outObj->rotation = element->FloatAttribute("rotation");
	outObj->referenceGid = element->UnsignedAttribute("gid");
	outObj->visible = element->BoolAttribute("visible");

	error = _parsePropertyNode(element->FirstChildElement("properties"), &outObj->propertyMap);
	if (error)
	{
		return error;
	}


	tinyxml2::XMLElement* shapeElement = NULL;
	if ((shapeElement = element->FirstChildElement("ellipse")) != NULL)
	{
		outObj->shapeType = kEllipse;
	}
	else if ((shapeElement = element->FirstChildElement("polygon")) != NULL)
	{
		outObj->shapeType = kPolygon;
	}
	else if ((shapeElement = element->FirstChildElement("polyline")) != NULL)
	{
		outObj->shapeType = kPolyline;
	}
	else
	{
		outObj->shapeType = kSquare;
	}

	if ((outObj->shapeType == kPolygon || outObj->shapeType == kPolyline) && shapeElement != NULL)
	{
		if (shapeElement->Attribute("points") == NULL)
		{
			LOGE("Missing points attribute for shape requiring one...");
			return TmxReturn::kErrorParsing;
		}

		std::string pointString = shapeElement->Attribute("points");
		std::istringstream pairStringStream(pointString);
		std::string pairToken;
		while(std::getline(pairStringStream, pairToken, ' '))
		{
			std::pair<int, int> pair;
			std::istringstream pointStringString(pairToken);
			std::string pointToken;
			std::getline(pointStringString, pointToken, ',');
			pair.first = atoi(pointToken.c_str());
			std::getline(pointStringString, pointToken, ',');
			pair.second = atoi(pointToken.c_str());

			outObj->shapePoints.push_back(pair);
		}
	}

	return error;
}


}
