/**
 *
 *
 */

#ifndef _LIB_TMX_PARSER_H_
#define _LIB_TMX_PARSER_H_


#include <string>
#include <unordered_map>
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



typedef std::unordered_map<std::string, std::string> TmxPropertyMap_t;


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
} Orientation;


typedef struct
{
	std::string name;
	std::string value;
} TmxProperty;


typedef struct
{
	unsigned int id;
	TmxPropertyMap_t propertyMap;
} TmxTileDefinition;


typedef std::vector<TmxTileDefinition> TmxTileDefinitionCollection_t;


typedef struct
{
	unsigned int x;
	unsigned int y;
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
	TmxTileDefinitionCollection_t _tiles;
} TmxTileset;


typedef std::vector<TmxTileset> TmxTilesetCollection_t;


typedef struct
{
	unsigned int gid;
	unsigned int tilesetIndex;
	unsigned int tileInTilesetIndex;
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
	std::string orientation;
	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	std::string backgroundColor;
	TmxPropertyMap_t propertyMap;
	TmxTilesetCollection_t tilesetCollection;
	TmxLayerCollection_t layerCollection;
} TmxMap;



/**
 * Parse a tmx from a filename.
 * @param fileName
 * @param outMap
 * @return
 */
TmxReturn parseFromFile(const std::string& fileName, TmxMap* outMap);


/**
 * Parse a tmx file from memory.
 * @param data Tmx file in memory
 * @param length Size of the data buffer
 * @param outMap
 * @return
 */
TmxReturn parseFromMemory(void* data, size_t length, TmxMap* outMap);


}
#endif /* _LIB_TMX_PARSER_H_ */
