/**
 *
 *
 */

#ifndef _LIB_TMX_PARSER_H_
#define _LIB_TMX_PARSER_H_


#include <string>
#include <unordered_map>

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
	std::string version;
	std::string orientation;
	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	std::string backgroundColor;
} TmxMap;


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


/*
 *
 */
class Tmx
{
public:
	Tmx();


	virtual ~Tmx();


protected:


private:


};


}
#endif /* _LIB_TMX_PARSER_H_ */
