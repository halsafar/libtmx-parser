

#include <iostream>

#include "src/tmxparser.h"


#define DEPTH_SCALE 5


#define printf_depth( depth, format, ... ) \
	printf("%*s " format "\n", ((depth)*DEPTH_SCALE), "", __VA_ARGS__)


void printProperties(int depth, const tmxparser::TmxPropertyMap_t& map)
{
	if (map.empty())
	{
		return;
	}

	printf_depth(depth, "%s", "<properties>");
	for (auto it = map.begin(); it != map.end(); ++it)
	{
		printf_depth(depth+1, "%s=%s", it->first.c_str(), it->second.c_str());
	}
}


void printObjects(int depth, const tmxparser::TmxObjectCollection_t& collection)
{
	int nextdepth = depth + 1;

	for (auto it = collection.begin(); it != collection.end(); ++it)
	{
		printf_depth(depth, "%s", "<object>");

		printf_depth(nextdepth, "Name: %s", it->name.c_str());
		printf_depth(nextdepth, "Type: %s", it->type.c_str());
		printf_depth(nextdepth, "x: %f", it->x);
		printf_depth(nextdepth, "y: %f", it->y);
		printf_depth(nextdepth, "width: %f", it->width);
		printf_depth(nextdepth, "height: %f", it->height);
		printf_depth(nextdepth, "rotation: %f", it->rotation);
		printf_depth(nextdepth, "refGid: %u", it->referenceGid);
		printf_depth(nextdepth, "visible: %u", it->visible);

		printProperties(nextdepth, it->propertyMap);

		printf_depth(nextdepth, "ShapeType: %u", it->shapeType);

		printf_depth(nextdepth, "%s", "<shape>");
		for (auto pointIt = it->shapePoints.begin(); pointIt != it->shapePoints.end(); ++pointIt)
		{
			printf_depth(nextdepth+1, "(x, y)=%f,%f", pointIt->first, pointIt->second);
		}
	}
}


void printObjectGroups(int depth, const tmxparser::TmxObjectGroupCollection_t& collection)
{
	int nextdepth = depth +1;

	for (auto it = collection.begin(); it != collection.end(); ++it)
	{
		printf_depth(depth, "%s", "<objectgroup>");

		printf_depth(nextdepth, "Name: %s", it->name.c_str());
		printf_depth(nextdepth, "Color: %s", it->color.c_str());
		printf_depth(nextdepth, "Opacity: %f", it->opacity);
		printf_depth(nextdepth, "Visible: %u", it->visible);
		printProperties(nextdepth, it->propertyMap);
		printObjects(nextdepth, it->objects);
	}
}


void printImageData(int depth, const tmxparser::TmxImage& tmximage)
{
	printf_depth(depth, "%s", "<image>");

	depth = depth +1;
	printf_depth(depth, "Source: %s", tmximage.source.c_str());
	printf_depth(depth, "Format: %s", tmximage.format.c_str());
	printf_depth(depth, "Width: %u", tmximage.width);
	printf_depth(depth, "Height: %u", tmximage.height);
	printf_depth(depth, "TransparentColor: %s", tmximage.transparentColor.c_str());
}


void printTileDefinition(int depth, const tmxparser::TmxTileDefinitionMap_t& map)
{
	for (auto it = map.begin(); it != map.end(); ++it)
	{
		printf_depth(depth, "%s", "<tile>");
		printf_depth(depth+1, "Id: %u", it->second.id);

		printProperties(depth+1, it->second.propertyMap);

		// animations
		printf_depth(depth+1, "%s", "<animation>");
		depth+=2;
		for (auto animIt = it->second.animations.begin(); animIt != it->second.animations.end(); ++animIt)
		{
			printf_depth(depth, "<frame tileId=%u duration=%f>", animIt->tileId, animIt->duration);
		}
		depth-=2;

		printObjectGroups(depth+1, it->second.objectgroups);
	}
}


void printTilesets(int depth, const tmxparser::TmxTilesetCollection_t& collection)
{
	for (auto it = collection.begin(); it != collection.end(); ++it)
	{
		printf_depth(depth, "%s", "<tileset>");

		int nextdepth = depth + 1;

		printf_depth(nextdepth, "Name: %s", (*it).name.c_str());
		printf_depth(nextdepth, "FirstGid: %u", (*it).firstgid);
		printf_depth(nextdepth, "Width: %u", (*it).tileWidth);
		printf_depth(nextdepth, "Height: %u", (*it).tileHeight);
		printf_depth(nextdepth, "MarginImage: %u", (*it).tileMarginInImage);
		printf_depth(nextdepth, "SpaceImage: %u", (*it).tileSpacingInImage);
		printf_depth(nextdepth, "rowCount: %u", (*it).colCount);
		printf_depth(nextdepth, "colCount: %u", (*it).rowCount);
		printImageData(nextdepth, (*it).image);
		printTileDefinition(nextdepth, (*it).tileDefinitions);
	}
}


void printLayerTiles(int depth, const tmxparser::TmxLayerTileCollection_t& collection)
{
	for (auto it = collection.begin(); it != collection.end(); ++it)
	{
		printf_depth(depth, "%s", "<tile>");

		int nextdepth = depth + 1;

		printf_depth(nextdepth, "Gid: %u", it->gid);
		printf_depth(nextdepth, "tilesetIndex: %u", it->tilesetIndex);
		printf_depth(nextdepth, "tileFlatIndex: %u", it->tileFlatIndex);
	}
}


void printLayers(int depth, const tmxparser::TmxLayerCollection_t& collection)
{
	for (auto it = collection.begin(); it != collection.end(); ++it)
	{
		printf_depth(depth, "%s", "<layer>");

		int nextdepth = depth + 1;

		printf_depth(nextdepth, "Name: %s", it->name.c_str());
		printf_depth(nextdepth, "Width: %u", (*it).width);
		printf_depth(nextdepth, "Height: %u", (*it).height);
		printf_depth(nextdepth, "Opacity: %f", (*it).opacity);
		printf_depth(nextdepth, "Visible: %u", (*it).visible);
		printProperties(nextdepth+1, it->propertyMap);
		printLayerTiles(nextdepth+1, it->tiles);
	}
}


void printTmxMapData(const tmxparser::TmxMap* map)
{
	int depth = 0;

	printf_depth(0, "%s", "<map>");
	depth = 1;
	printf_depth(depth, "Version: %s", map->version.c_str());
	printf_depth(depth, "Orientation: %d", map->orientation);
	printf_depth(depth, "Width: %u", map->width);
	printf_depth(depth, "Height: %u", map->height);
	printf_depth(depth, "TileWidth: %u", map->tileWidth);
	printf_depth(depth, "TileHeight: %u", map->tileHeight);
	printf_depth(depth, "BackgroundColor: %s", map->backgroundColor.c_str());
	printProperties(depth+1, map->propertyMap);
	printTilesets(depth+1, map->tilesetCollection);
	printLayers(depth+1, map->layerCollection);
	printObjectGroups(depth+1, map->objectGroupCollection);
}


int main()
{
	printf("tmxparser::main()\n");

	tmxparser::TmxReturn error;
	tmxparser::TmxMap map;

	// test from file
	error = tmxparser::parseFromFile("./test_files/test_xml_level.tmx", &map, "test/textures/");

	// test from memory
	/*FILE* fp;
	fp = fopen("example.tmx", "rb");
	fseek( fp, 0, SEEK_SET );
	fgetc( fp );
	fseek( fp, 0, SEEK_END );
	size_t size = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	char* charBuffer = new char[size+1];
	size_t read = fread( charBuffer, 1, size, fp );

	t = tmxparser::parseFromMemory(charBuffer, size, &map);*/

	if (!error)
	{
		printTmxMapData(&map);

		tmxparser::TmxRect rect;
		rect.u = 0; rect.v = 0; rect.u2 = 0; rect.v2 = 0;
		for (auto it : map.layerCollection[0].tiles)
		{
			tmxparser::calculateTileCoordinatesUV(map.tilesetCollection[it.tilesetIndex], it.tileFlatIndex, 0.5f, true, rect);
			printf("Tileset[%u]@Tile[%u]=Rect( (%f, %f)->(%f, %f) )\n", it.tilesetIndex, it.tileFlatIndex, rect.u, rect.v, rect.u2, rect.v2);
		}
	}
	else
	{
		printf("error parsing file");
	}

	return error;
}



