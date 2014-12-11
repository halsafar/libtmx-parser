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

#ifndef _LIB_TMX_PARSER_H_
#define _LIB_TMX_PARSER_H_


#include <string>

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <unordered_map>
#else
#include <map>
#endif

#include <vector>

#include <tinyxml2.h>


namespace tmxparser
{


typedef enum
{
	kSuccess,
	kErrorParsing,
	kMissingRequiredAttribute,
	kMissingMapNode,
	kMissingDataNode,
	kMalformedPropertyNode,
} TmxReturn;


#ifdef __GXX_EXPERIMENTAL_CXX0X__
typedef std::unordered_map<std::string, std::string> TmxPropertyMap_t;
#else
typedef std::map<std::string, std::string> TmxPropertyMap_t;
#endif


typedef unsigned int TileId_t;


/**
 * Used to identify tmx file encoding type for data tags
 */
typedef enum
{
	kEncodingXml,   //!< No encoding in tiled means XML
	kEncodingBase64,//!< kEncodingBase64
	kEncodingCsv    //!< kEncodingCsv
} TmxDataNodeEncodingType;


typedef enum
{
	kCompressionNone,
	kCompressionZlib,
	kCompressionGzip,
} TmxDataCompressionType;


typedef enum
{
	kOrthogonal,
	kIsometric,
	kStaggered
} TmxOrientation;


typedef enum
{
	kPolygon,
	kPolyline,
	kEllipse,
	kSquare,
} TmxShapeType;


typedef struct
{
	std::string name;
	std::string value;
} TmxProperty;


typedef struct
{
	TileId_t tileId;
	float duration;
} TmxAnimationFrame;


typedef std::vector<TmxAnimationFrame> TmxAnimationFrameCollection_t;


typedef struct
{
	TileId_t id;

	TmxPropertyMap_t propertyMap;
	TmxAnimationFrameCollection_t animations;
	// objectgroups
} TmxTileDefinition;


typedef std::unordered_map<unsigned int, TmxTileDefinition> TmxTileDefinitionMap_t;


typedef struct
{
	int x;
	int y;
} TmxTileOffset;


typedef struct
{
	std::string format;
	std::string source;
	std::string transparentColor;
	unsigned int width;
	unsigned int height;
} TmxImage;


typedef struct
{
	unsigned int firstgid;
	std::string name;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileSpacingInImage;
	unsigned int tileMarginInImage;
	TmxImage image;
	TmxTileDefinitionMap_t tileDefinitions;
} TmxTileset;


typedef std::vector<TmxTileset> TmxTilesetCollection_t;


typedef struct
{
	unsigned int gid;
	unsigned int tilesetIndex;
	unsigned int tileFlatIndex;
	unsigned int tileXIndex;
	unsigned int tileYIndex;
} TmxLayerTile;


typedef std::vector<TmxLayerTile> TmxLayerTileCollection_t;


typedef struct
{
	std::string name;
	unsigned int width;
	unsigned int height;
	float opacity;
	bool visible;
	TmxPropertyMap_t propertyMap;
	TmxLayerTileCollection_t tiles;
} TmxLayer;


typedef std::vector<TmxLayer> TmxLayerCollection_t;


typedef std::pair<float, float> TmxShapePoint;


typedef std::vector<TmxShapePoint> TmxShapePointCollection_t;


typedef struct
{
	std::string name;
	std::string type;
	float x;
	float y;
	unsigned int width;
	unsigned int height;
	float rotation;
	unsigned int referenceGid;
	bool visible;
	TmxPropertyMap_t propertyMap;
	TmxShapeType shapeType;
	TmxShapePointCollection_t shapePoints;
} TmxObject;


typedef std::vector<TmxObject> TmxObjectCollection_t;


typedef struct
{
	std::string name;
	std::string color;
	float opacity;
	bool visible;
	TmxPropertyMap_t propertyMap;
	TmxObjectCollection_t objects;
} TmxObjectGroup;


typedef std::vector<TmxObjectGroup> TmxObjectGroupCollection_t;


typedef struct
{
	std::string version;
	std::string orientation;
	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	std::string backgroundColor;
	std::string renderOrder;
	TmxPropertyMap_t propertyMap;
	TmxTilesetCollection_t tilesetCollection;
	TmxLayerCollection_t layerCollection;
	TmxObjectGroupCollection_t objectGroupCollection;
} TmxMap;



/**
 * Parse a tmx from a filename.
 * @param fileName
 * @param outMap
 * @return
 */
TmxReturn parseFromFile(const std::string& fileName, TmxMap* outMap, const std::string& tilesetPath);


/**
 * Parse a tmx file from memory.
 * @param data Tmx file in memory
 * @param length Size of the data buffer
 * @param outMap
 * @return
 */
TmxReturn parseFromMemory(void* data, size_t length, TmxMap* outMap, const std::string& tilesetPath);


}
#endif /* _LIB_TMX_PARSER_H_ */
