/**
 *
 *
 */

#ifndef _LIB_TMX_PARSER_H_
#define _LIB_TMX_PARSER_H_


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <tinyxml2.h>


namespace TmxParser
{


typedef std::unordered_map<std::string, std::string> TmxPropertyMap_t;


typedef enum
{
	kEncodingXml,
	kEncodingBase64,
	kEncodingCsv
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


/*
 *
 */
class Tmx
{
public:
	Tmx();


	virtual ~Tmx();


	std::unique_ptr<TmxMap> parseFromFile(const std::string& fileName);


	std::unique_ptr<TmxMap> parseFromMemory(void* data, size_t length);


	//unsigned int gidToLayer
protected:


private:
	std::unique_ptr<TmxMap> _parseMapNode(tinyxml2::XMLElement* element);


	TmxPropertyMap_t _parsePropertyNode(tinyxml2::XMLElement* element);


	TmxImage _parseImageNode(tinyxml2::XMLElement* element);


	TmxTileset _parseTilesetNode(tinyxml2::XMLElement* element);


	TmxTileDefinition _parseTileDefinitionNode(tinyxml2::XMLElement* element);


	TmxLayer _parseLayerNode(tinyxml2::XMLElement* element);


	TmxLayerTileCollection_t _parseLayerXmlDataNode(tinyxml2::XMLElement* element);


	TmxLayerTile _parseLayerXmlTileNode(tinyxml2::XMLElement* element);


	TmxLayerTileCollection_t _parseLayerCsvDataNode(tinyxml2::XMLElement* element);


	TmxLayerTileCollection_t _parseLayerBase64DataNode(tinyxml2::XMLElement* element);
};


}
#endif /* _LIB_TMX_PARSER_H_ */
