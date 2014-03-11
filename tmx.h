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


typedef struct
{
	unsigned int x;
	unsigned int y;
} TmxTileOffset;


typedef struct
{
	unsigned int firstgid;
	std::string name;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileSpacingInImage;
	unsigned int tileMarginInImage;
} TmxTileset;


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
	unsigned int gid;
} TmxLayerTile;


typedef struct
{
	std::string name;
	unsigned int width;
	unsigned int height;
	float opacity;
	bool visible;
} TmxLayer;


typedef std::vector<std::shared_ptr<TmxLayer>> TmxLayerCollection_t;


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
};


}
#endif /* _LIB_TMX_PARSER_H_ */
