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

#if defined __GXX_EXPERIMENTAL_CXX0X__ || (_MSC_VER >= 1800)
#include <unordered_map>
template <typename TKey, typename TValue>
struct Map
{
	typedef std::unordered_map<TKey, TValue> type;
};
#else
#include <map>
template <typename TKey, typename TValue>
struct Map
{
	typedef std::map<TKey, TValue> type;
};
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
	kInvalidTileIndex,
	kUnknownTileIndices,
} TmxReturn;


typedef Map<std::string, std::string>::type TmxPropertyMap_t;


typedef unsigned int TileId_t;


typedef enum
{
	kOrthogonal = 0,
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
	float u;
	float v;
	float u2;
	float v2;
} TmxRect;


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


typedef std::pair<float, float> TmxShapePoint;


typedef std::vector<TmxShapePoint> TmxShapePointCollection_t;


typedef struct
{
	std::string name;
	std::string type;
	float x;
	float y;
	float width;
	float height;
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
	TileId_t id;
	TmxPropertyMap_t propertyMap;
	TmxAnimationFrameCollection_t animations;
	TmxObjectGroupCollection_t objectgroups;
} TmxTileDefinition;


typedef Map<unsigned int, TmxTileDefinition>::type TmxTileDefinitionMap_t;


typedef struct
{
	int x;
	int y;
} TmxOffset;


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
	std::string name;
	unsigned int x;
	unsigned int y;
	unsigned int widthInTiles;
	unsigned int heightInTiles;
	float opacity;
	bool visible;
	TmxPropertyMap_t propertyMap;
	TmxImage image;
} TmxImageLayer;


typedef std::vector<TmxImageLayer> TmxImageLayerCollection_t;


typedef struct
{
	unsigned int firstgid;
	std::string name;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileSpacingInImage;
	unsigned int tileMarginInImage;
	TmxOffset offset;

	unsigned int rowCount; /// based on image width and tile spacing/margins
	unsigned int colCount; /// based on image height and tile spacing/margins

	TmxImage image;
	TmxTileDefinitionMap_t tileDefinitions;
} TmxTileset;


typedef std::vector<TmxTileset> TmxTilesetCollection_t;


typedef struct
{
	unsigned int gid;
	unsigned int tilesetIndex;
	unsigned int tileFlatIndex;
	bool flipX, flipY, flipDiagonal;
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


typedef struct
{
	std::string version;
	TmxOrientation orientation;
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
	TmxImageLayerCollection_t imageLayerCollection;
} TmxMap;



/**
 * Parse a tmx from a filename.
 * @param fileName Relative or Absolute filename to the TMX file to load.
 * @param outMap An allocated TmxMap object ready to be populated.
 * @return kSuccess on success.
 */
TmxReturn parseFromFile(const std::string& fileName, TmxMap* outMap, const std::string& tilesetPath);


/**
 * Parse a tmx file from memory.  Useful for Android or IOS.
 * @param data Tmx file in memory, still in its xml format just already loaded.
 * @param length Size of the data buffer.
 * @param outMap An allocated TmxMap object ready to be populated.
 * @return kSuccess on success.
 */
TmxReturn parseFromMemory(void* data, size_t length, TmxMap* outMap, const std::string& tilesetPath);


/**
 * Takes a tileset and an index with that tileset and generates OpenGL/DX ready texture coordinates.
 * @param tileset A tileset to use for generating coordinates.
 * @param tileFlatIndex The flat index into the tileset.
 * @param pixelCorrection The amount to use for pixel correct, 0.5f is typical, see 'half-pixel correction'.
 * @param outRect Contains the four corners of the tile.
 * @return kSuccess on success.
 */
TmxReturn calculateTileCoordinatesUV(const TmxTileset& tileset,  unsigned int tileFlatIndex, float pixelCorrection, bool flipY, TmxRect& outRect);


}
#endif /* _LIB_TMX_PARSER_H_ */
