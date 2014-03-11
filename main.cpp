

#include <iostream>

#include "tmx.h"


#define DEPTH_SCALE 4


#define printf_depth( depth, format, ... ) \
	printf("%*s " format "\n", depth*DEPTH_SCALE, "", __VA_ARGS__)


void printProperties(int depth, const TmxParser::TmxPropertyMap_t& map)
{
	printf_depth(depth, "%s", "<properties>");
	for (auto it = map.begin(); it != map.end(); ++it)
	{
		printf_depth(depth+2, "%s=%s", it->first.c_str(), it->second.c_str());
	}
}


void printTmxMapData(const TmxParser::TmxMap* map)
{
	int depth = 0;

	printf_depth(0, "%s", "<map>");
	depth = 1;
	printf_depth(depth, "Version: %s", map->version.c_str());
	printf_depth(depth, "Orientation: %s", map->orientation.c_str());
	printf_depth(depth, "Width: %u", map->width);
	printf_depth(depth, "Height: %u", map->height);
	printf_depth(depth, "TileWidth: %u", map->tileWidth);
	printf_depth(depth, "TileHeight: %u", map->tileHeight);
	printf_depth(depth, "BackgroundColor: %s", map->backgroundColor.c_str());
	printProperties(depth+1, map->propertyMap);
}


int main()
{
	std::cout << "Hello World" << std::endl;

	TmxParser::Tmx tmx;
	std::unique_ptr<TmxParser::TmxMap> map = tmx.parseFromFile("example.tmx");

	printTmxMapData(map.get());

	return 0;
}



