/*
The MIT License (MIT)

Copyright (c) 2014 Stephen Damm - shinhalsafar@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "tmxparser.h"

#include "base64.h"


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

#include <algorithm>
#include <string>
#include <sstream>


#ifndef LOG_TAG
#define LOG_TAG "libtmxparser"
#endif


#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#define PATH_SEPARATOR_ALT "/"
#else
#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_ALT "/" // needs value!
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


namespace tmxparser
{


#define CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(XMLELEMENT, ATTRIBNAME, LHS) \
	if (XMLELEMENT->Attribute(ATTRIBNAME) != NULL) \
	{ \
		LHS = XMLELEMENT->Attribute(ATTRIBNAME); \
	} \
	else \
	{ \
		LHS = '\0'; \
	}


#define CHECK_AND_RETRIEVE_REQ_ATTRIBUTE_STRING(XMLELEMENT, ATTRIBNAME, LHS) \
	LHS = XMLELEMENT->Attribute(ATTRIBNAME); \
	if (LHS.size() == 0) \
	{ \
		LOGE("Missing required attribute [%s]", ATTRIBNAME); \
		return TmxReturn::kMissingRequiredAttribute; \
	}


#define CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(FUNC, ATTRIBNAME, OUT) \
		if (FUNC(ATTRIBNAME, OUT) == tinyxml2::XML_NO_ATTRIBUTE) \
		{ \
			LOGE("Missing required attribute [%s]", ATTRIBNAME); \
			return kMissingRequiredAttribute; \
		}


// Prototypes
TmxReturn _parseStart(tinyxml2::XMLElement* element, TmxMap* outMap, const std::string& tilesetPath);
TmxReturn _parseEnd(TmxMap* outMap, const std::string& tilesetPath);
void _parseEndHelper(TmxImage& image, const std::string& tilesetPath);
TmxReturn _parseMapNode(tinyxml2::XMLElement* element, TmxMap* outMap);
TmxReturn _parsePropertyNode(tinyxml2::XMLElement* element, TmxPropertyMap_t* outPropertyMap);
TmxReturn _parseImageNode(tinyxml2::XMLElement* element, TmxImage* outImage);
TmxReturn _parseTilesetNode(tinyxml2::XMLElement* element, TmxTileset* outTileset);
TmxReturn _parseTileDefinitionNode(tinyxml2::XMLElement* element, TmxTileDefinition* outTileDefinition);
TmxReturn _parseTileAnimationNode(tinyxml2::XMLElement* element, TmxAnimationFrameCollection_t* outAnimationCollection);
TmxReturn _parseLayerNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayer* outLayer);
TmxReturn _parseLayerDataNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTileCollection_t* outTileCollection);
TmxReturn _parseLayerXmlTileNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile);
TmxReturn _calculateTileIndices(const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile);
TmxReturn _parseObjectGroupNode(tinyxml2::XMLElement* element, TmxObjectGroup* outObjectGroup);
TmxReturn _parseObjectNode(tinyxml2::XMLElement* element, TmxObject* outObj);
TmxReturn _parseOffsetNode(tinyxml2::XMLElement* element, TmxOffset* offset);
TmxReturn _parseImageLayerNode(tinyxml2::XMLElement* element, TmxImageLayer* outImageLayer);

TmxReturn parseFromFile(const std::string& fileName, TmxMap* outMap, const std::string& tilesetPath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(fileName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		LOGE("Cannot read xml file");
		return TmxReturn::kErrorParsing;
	}

	// parse the map node
	return _parseStart(doc.FirstChildElement("map"), outMap, tilesetPath);
}


TmxReturn parseFromMemory(void* data, size_t length, TmxMap* outMap, const std::string& tilesetPath)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse((char*)data, length))
	{
		LOGE("Cannot parse xml memory file...");
		return TmxReturn::kErrorParsing;
	}

	return _parseStart(doc.FirstChildElement("map"), outMap, tilesetPath);
}


TmxReturn _parseStart(tinyxml2::XMLElement* element, TmxMap* outMap, const std::string& tilesetPath)
{
	TmxReturn retVal = _parseMapNode(element, outMap);
	return (retVal == TmxReturn::kSuccess) ? _parseEnd(outMap, tilesetPath) : retVal;
}


TmxReturn _parseEnd(TmxMap* outMap, const std::string& tilesetPath)
{
	for (auto tileIt = outMap->tilesetCollection.begin(); tileIt != outMap->tilesetCollection.end(); ++tileIt)
		_parseEndHelper(tileIt->image, tilesetPath);

	for (auto tileIt = outMap->imageLayerCollection.begin(); tileIt != outMap->imageLayerCollection.end(); ++tileIt)
		_parseEndHelper(tileIt->image, tilesetPath);

	return TmxReturn::kSuccess;
}


void _parseEndHelper(TmxImage& image, const std::string& tilesetPath)
{
	auto pathSeperatorMissing = (image.source.find(PATH_SEPARATOR) == std::string::npos);
	auto pathSeperatorAltMissing = (image.source.find(PATH_SEPARATOR_ALT) == std::string::npos);

	if (pathSeperatorMissing && pathSeperatorAltMissing) // no path_seperator found --> relative path
	{
		std::string baseFilename = image.source.substr(image.source.find_last_of(PATH_SEPARATOR) + 1);

		/*
		 * here we find out what separator is used in tilesetPath and choose it
		 * if both (alternative and normal) separators are used, we always use the normal separator
		 **/
		auto seperatorThere = (tilesetPath.find(PATH_SEPARATOR) != std::string::npos);
		auto seperatorAltThere = (tilesetPath.find(PATH_SEPARATOR_ALT) != std::string::npos);
		auto tileSetPathSeparator = PATH_SEPARATOR;

		if (!seperatorThere && seperatorAltThere) // only alternative separator exists in tilesetPath --> choose it
			tileSetPathSeparator = PATH_SEPARATOR_ALT;

		image.source = tilesetPath + tileSetPathSeparator + baseFilename;
	}
}


TmxReturn _parseMapNode(tinyxml2::XMLElement* element, TmxMap* outMap)
{
	if (element == NULL)
	{
		return TmxReturn::kMissingMapNode;
	}

	outMap->version = element->Attribute("version");
	const char* orientation = element->Attribute("orientation");
	if (orientation != NULL)
	{
		if (strcmp(orientation, "orthogonal") == 0)
		{
			outMap->orientation = TmxOrientation::kOrthogonal;
		}
		else if (strcmp(orientation, "isometric") == 0)
		{
			outMap->orientation = TmxOrientation::kIsometric;
		}
		else if (strcmp(orientation, "staggered") == 0)
		{
			outMap->orientation = TmxOrientation::kStaggered;
		}
	}
	else
	{
		LOGW("Missing orientation attribute");
	}

	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "width", &outMap->width);
	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "height", &outMap->height);
	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "tilewidth", &outMap->tileWidth);
	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "tileheight", &outMap->tileHeight);

	CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(element, "backgroundcolor", outMap->backgroundColor);
	CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(element, "renderorder", outMap->renderOrder);


	TmxReturn error = _parsePropertyNode(element->FirstChildElement("properties"), &outMap->propertyMap);
	if (error)
	{
		LOGE("Error processing map properties...");
		return error;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("tileset"); child != NULL; child = child->NextSiblingElement("tileset"))
	{
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
			LOGE("Error processing objectgroup node...");
			return error;
		}

		outMap->objectGroupCollection.push_back(group);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("imagelayer"); child != NULL; child = child->NextSiblingElement("imagelayer"))
	{
		TmxImageLayer imageLayer;
		error = _parseImageLayerNode(child, &imageLayer);
		if (error)
		{
			LOGE("Error parsing imagelayer node...");
			return error;
		}

		outMap->imageLayerCollection.push_back(imageLayer);
	}

	return error;
}


TmxReturn _parsePropertyNode(tinyxml2::XMLElement* element, TmxPropertyMap_t* outPropertyMap)
{
	if (element == NULL)
	{
		// ignore this, not everything requires properties
		return TmxReturn::kSuccess;
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("property"); child != NULL; child = child->NextSiblingElement("property"))
	{
		if (strcmp(child->Name(), "property") == 0)
		{
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
	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE_STRING(element, "source", outImage->source);
	CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(element, "format", outImage->format);
	CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(element, "trans", outImage->transparentColor);

	outImage->width = element->UnsignedAttribute("width");
	outImage->height = element->UnsignedAttribute("height");

	return TmxReturn::kSuccess;
}


TmxReturn _parseTilesetNode(tinyxml2::XMLElement* element, TmxTileset* outTileset)
{

	if (strcmp(element->Name(), "tileset") == 0)
	{
		CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "firstgid", &outTileset->firstgid);
		outTileset->name = element->Attribute("name");
		CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "tilewidth", &outTileset->tileWidth);
		CHECK_AND_RETRIEVE_REQ_ATTRIBUTE(element->QueryUnsignedAttribute, "tileheight", &outTileset->tileHeight);
		outTileset->tileSpacingInImage = element->UnsignedAttribute("spacing");
		outTileset->tileMarginInImage = element->UnsignedAttribute("margin");

		// TODO - read TODO at end of this function
		if (element->FirstChildElement("image") == NULL)
		{
			LOGE("We do not support maps with tilesets that have no image associated currently...");
			return kErrorParsing;
		}

		TmxImage image;
		TmxReturn error = _parseImageNode(element->FirstChildElement("image"), &outTileset->image);
		if (error)
		{
			LOGE("Error parsing image node...");
			return error;
		}

		outTileset->offset.x = 0;
		outTileset->offset.y = 0;

		if (element->FirstChildElement("tileoffset") != NULL)
		{
			error = _parseOffsetNode(element->FirstChildElement("tileoffset"), &outTileset->offset);
		}

		for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
		{
			TmxTileDefinition tileDef;

			tileDef.id = 0;

			error = _parseTileDefinitionNode(child, &tileDef);
			if (error)
			{
				LOGE("Error parsing tile definition");
				return error;
			}

			outTileset->tileDefinitions[tileDef.id] = tileDef;
		}

		// derive row/col count, calculate tile indices
		// TODO - this is why we do not accept tilesets without image tag atm
		outTileset->colCount = (outTileset->image.width - outTileset->tileMarginInImage) / (outTileset->tileWidth + outTileset->tileSpacingInImage);
		outTileset->rowCount = (outTileset->image.height - outTileset->tileMarginInImage) / (outTileset->tileHeight + outTileset->tileSpacingInImage);
	}

	return TmxReturn::kSuccess;
}


TmxReturn _parseTileDefinitionNode(tinyxml2::XMLElement* element, TmxTileDefinition* outTileDefinition)
{
	TmxReturn error = TmxReturn::kSuccess;

	outTileDefinition->id = element->UnsignedAttribute("id");
	error = _parsePropertyNode(element->FirstChildElement("properties"), &outTileDefinition->propertyMap);
	if (error)
	{
		return error;
	}

	if (element->FirstChildElement("animation") != NULL)
	{
		error = _parseTileAnimationNode(element->FirstChildElement("animation"), &outTileDefinition->animations);
	}

	for (tinyxml2::XMLElement* child = element->FirstChildElement("objectgroup"); child != NULL; child = child->NextSiblingElement("objectgroup"))
	{
		TmxObjectGroup group;
		error = _parseObjectGroupNode(child, &group);
		if (error)
		{
			LOGE("Error processing objectgroup node...");
			return error;
		}

		outTileDefinition->objectgroups.push_back(group);
	}

	return error;
}


TmxReturn _parseTileAnimationNode(tinyxml2::XMLElement* element, TmxAnimationFrameCollection_t* outAnimationCollection)
{
	for (tinyxml2::XMLElement* child = element->FirstChildElement("frame"); child != NULL; child = child->NextSiblingElement("frame"))
	{
		TmxAnimationFrame frame;
		frame.duration = child->FloatAttribute("duration");
		frame.tileId = child->UnsignedAttribute("tileid");

		outAnimationCollection->push_back(frame);
	}

	return kSuccess;
}


TmxReturn _parseLayerNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayer* outLayer)
{
	TmxReturn error = TmxReturn::kSuccess;

	outLayer->name = element->Attribute("name");
	if (element->Attribute("opacity"))
		outLayer->opacity = element->FloatAttribute("opacity");
	else
		outLayer->opacity = 1.f;
	if (element->Attribute("visible"))
		outLayer->visible = (element->IntAttribute("visible") == 1 ? true : false);
	else
		outLayer->visible = true;
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
		error = _parseLayerDataNode(dataElement, tilesets, &outLayer->tiles);
	}
	else
	{
		LOGE("Layer missing data node...");
		return TmxReturn::kMissingDataNode;
	}

	return error;
}


TmxReturn _parseLayerDataNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTileCollection_t* outTileCollection)
{
	TmxReturn error = TmxReturn::kSuccess;

	const char* encoding = element->Attribute("encoding");
	const char* compression = element->Attribute("compression");

	if (compression != NULL)
	{
		LOGE("Does not support compression yet...");
		return TmxReturn::kErrorParsing;
	}

	if (encoding == NULL)
	{
		for (tinyxml2::XMLElement* child = element->FirstChildElement("tile"); child != NULL; child = child->NextSiblingElement("tile"))
		{
			TmxLayerTile tile;
			error = _parseLayerXmlTileNode(child, tilesets, &tile);
			outTileCollection->push_back(tile);
		}
	}
	else if (strcmp(encoding, "csv") == 0)
	{
		std::stringstream csvss(element->FirstChild()->Value());
		//LOGE("TEXT = %s", csv.c_str());

		unsigned int gid = 0;
		while (csvss >> gid)
		{
			if (csvss.peek() == ',' || csvss.peek() == '\n')
			{
				csvss.ignore();
			}

			TmxLayerTile tile;

			tile.gid = gid;
			error = _calculateTileIndices(tilesets, &tile);
			if (error == TmxReturn::kErrorParsing)
			{
				return error;
			}

			outTileCollection->push_back(tile);
		}
	}
	else if (strcmp(encoding, "base64") == 0)
	{
		std::string csvbase64 = element->FirstChild()->Value();

		csvbase64.erase(std::remove(csvbase64.begin(), csvbase64.end(), '\n'), csvbase64.end());
		csvbase64.erase(std::remove(csvbase64.begin(), csvbase64.end(), '\r'), csvbase64.end());
		csvbase64.erase(std::remove(csvbase64.begin(), csvbase64.end(), ' '), csvbase64.end());

		std::string csv = base64_decode(csvbase64);

		unsigned int length = csv.size() / sizeof(unsigned int);

		// tiled base64 layer data is an unsigned 32bit array little endian
		// TODO - verify this on other platforms, write some tests
		unsigned int* p = (unsigned int*)csv.c_str();
		for (unsigned int i = 0; i < length; i++)
		{
			TmxLayerTile tile;
			tile.gid = p[i];
			error = _calculateTileIndices(tilesets, &tile);
			if (error == TmxReturn::kErrorParsing)
			{
				return error;
			}

			outTileCollection->push_back(tile);
		}
	}
	else
	{
		LOGE("Unsupported layer compression [%s]... coming soon...", encoding);
		return TmxReturn::kErrorParsing;
	}

	return error;
}



TmxReturn _parseLayerXmlTileNode(tinyxml2::XMLElement* element, const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile)
{
	TmxReturn error = TmxReturn::kSuccess;

	unsigned int gid = element->UnsignedAttribute("gid");

	unsigned int flipXFlag = 0x80000000;
	unsigned int flipYFlag = 0x40000000;
	unsigned int flipDiagonalFlag = 0x20000000;

	outTile->flipX = (gid & flipXFlag ? true : false);
	outTile->flipY = (gid & flipYFlag ? true : false);
	outTile->flipDiagonal = (gid & flipDiagonalFlag ? true : false);
	outTile->gid = (gid & ~(flipXFlag | flipYFlag | flipDiagonalFlag));

	return _calculateTileIndices(tilesets, outTile);
}


TmxReturn _calculateTileIndices(const TmxTilesetCollection_t& tilesets, TmxLayerTile* outTile)
{
	outTile->tilesetIndex = 0;
	outTile->tileFlatIndex = 0;

	// exit quickly
	if (outTile->gid == 0)
	{
		return TmxReturn::kSuccess;
	}

	// search for tilesetindex
	// O(n) where n = number of tilesets
	// Generally n will never be high but this method is called from an O(m) method.
	unsigned int index = 0;
	unsigned int lastEndIndex = 1;
	for (auto it = tilesets.begin(); it != tilesets.end(); ++it)
	{
		unsigned int colCount = it->colCount;
		unsigned int rowCount = it->rowCount;
		unsigned int startIndex = it->firstgid;
		unsigned int endIndex = it->firstgid + ( colCount * rowCount );

		if (outTile->gid >= startIndex && outTile->gid < endIndex)
		{
			outTile->tilesetIndex = index;
			outTile->tileFlatIndex = (outTile->gid) - lastEndIndex;

			// done
			return TmxReturn::kSuccess;
		}

		lastEndIndex = endIndex;

		index++;
	}

	// if we made it here, tile indices could not be found
	return TmxReturn::kUnknownTileIndices;
}


TmxReturn _parseObjectGroupNode(tinyxml2::XMLElement* element, TmxObjectGroup* outObjectGroup)
{
	TmxReturn error = TmxReturn::kSuccess;

	CHECK_AND_RETRIEVE_OPT_ATTRIBUTE_STRING(element, "name", outObjectGroup->name);

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
		outObjectGroup->visible = true;
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
	outObj->x = element->FloatAttribute("x");
	outObj->y = element->FloatAttribute("y");
	outObj->width = element->FloatAttribute("width");
	outObj->height = element->FloatAttribute("height");
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
			TmxShapePoint pair;
			std::istringstream pointStringString(pairToken);
			std::string pointToken;
			std::getline(pointStringString, pointToken, ',');
			pair.first = (float)atof(pointToken.c_str());
			std::getline(pointStringString, pointToken, ',');
			pair.second = (float)atof(pointToken.c_str());

			outObj->shapePoints.push_back(pair);
		}
	}

	return error;
}


TmxReturn calculateTileCoordinatesUV(const TmxTileset& tileset,  unsigned int tileFlatIndex, float pixelCorrection, bool flipY, TmxRect& outRect)
{
	if (tileFlatIndex >= tileset.colCount * tileset.rowCount)
	{
		return TmxReturn::kInvalidTileIndex;
	}

	TileId_t xIndex = tileFlatIndex % tileset.colCount;
	TileId_t yIndex = tileFlatIndex / tileset.colCount;

	unsigned int widthDelta = tileset.tileSpacingInImage + tileset.tileMarginInImage*xIndex;
	unsigned int heightDelta = tileset.tileSpacingInImage + tileset.tileMarginInImage*yIndex;

	float u = (float)((xIndex * tileset.tileWidth) + widthDelta + pixelCorrection) / (float)tileset.image.width;
	float v = (float)((yIndex * tileset.tileHeight) + heightDelta + pixelCorrection) / (float)tileset.image.height;

	float u2 = (float)((( (xIndex+1) * tileset.tileWidth) + widthDelta) - pixelCorrection) / (float)tileset.image.width;
	float v2 = (float)((( (yIndex+1) * tileset.tileHeight) + heightDelta) - pixelCorrection) / (float)tileset.image.height;

	if (flipY)
	{
		float tmpV = v;
		v = 1.f - v2;
		v2 = 1.f - tmpV;
	}

	outRect.u = u;
	outRect.v = v;
	outRect.u2 = u2;
	outRect.v2 = v2;

	return kSuccess;
}

tmxparser::TmxReturn _parseOffsetNode(tinyxml2::XMLElement* element, TmxOffset* offset)
{
	TmxReturn error = TmxReturn::kSuccess;

	offset->x = 0;
	offset->y = 0;

	if (element->Attribute("x"))
	{
		offset->x = element->IntAttribute("x");
	}

	if (element->Attribute("y"))
	{
		offset->y = element->IntAttribute("y");
	}

	return error;
}

tmxparser::TmxReturn _parseImageLayerNode(tinyxml2::XMLElement* element, TmxImageLayer* outImageLayer)
{
	TmxReturn error = TmxReturn::kSuccess;

	CHECK_AND_RETRIEVE_REQ_ATTRIBUTE_STRING(element, "name", outImageLayer->name);
	
	// x : optional , default 0
	if (element->QueryUnsignedAttribute("x", &outImageLayer->x) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->x = 0U;

	// y : optional , default 0
	if (element->QueryUnsignedAttribute("y", &outImageLayer->y) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->y = 0U;

	// width: optional, default: 0
	if (element->QueryUnsignedAttribute("width", &outImageLayer->widthInTiles) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->widthInTiles = 0U;
	
	// height: optional, default: 0
	if (element->QueryUnsignedAttribute("height", &outImageLayer->heightInTiles) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->heightInTiles = 0U;

	// opacity: optional, default: 1.0
	if (element->QueryFloatAttribute("opacity", &outImageLayer->opacity) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->opacity = 1.f;

	// visible: optional, default: true
	if (element->QueryBoolAttribute("visible", &outImageLayer->visible) == tinyxml2::XML_NO_ATTRIBUTE)
		outImageLayer->visible = true;

	// properties: optional
	if (element->FirstChildElement("properties") != NULL)
	{
		error = _parsePropertyNode(element->FirstChildElement("properties"), &outImageLayer->propertyMap);
		if (error != kSuccess)
		{
			LOGE("Error parsing image layer property node...");
			return error;
		}
	}

	// image: optional
	if (element->FirstChildElement("image") != NULL)
	{
		error = _parseImageNode(element->FirstChildElement("image"), &outImageLayer->image);
		if (error != kSuccess)
			return error;
	}

	return error;
}

}

