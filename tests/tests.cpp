#include "gtest/gtest.h"
#include "../src/tmxparser.h"

class TmxParseTest : public ::testing::Test
{
protected:
	static void SetUpTestCase()
	{
		_map = new tmxparser::TmxMap();
		tmxparser::parseFromFile("../test_files/test_xml_level.tmx", _map, "assets/textures/");
	}


	static void TearDownTestCase()
	{
		delete _map;
		_map = NULL;
	}


	virtual void SetUp()
	{

	}


	virtual void TearDown()
	{

	}


	static tmxparser::TmxMap* _map;
};


tmxparser::TmxMap* TmxParseTest::_map = NULL;


TEST_F(TmxParseTest, MapNotNull)
{
	ASSERT_TRUE(_map != NULL);
}


TEST_F(TmxParseTest, MapProperties)
{
	ASSERT_TRUE(_map != NULL);
	ASSERT_EQ("1.0", _map->version);
	ASSERT_EQ("orthogonal", _map->orientation);
	ASSERT_EQ("right-down", _map->renderOrder);
	ASSERT_EQ(10, _map->width);
	ASSERT_EQ(10, _map->height);
	ASSERT_EQ(16, _map->tileWidth);
	ASSERT_EQ(16, _map->tileHeight);
}


TEST_F(TmxParseTest, TilesetProperties)
{
	ASSERT_EQ(2, _map->tilesetCollection.size());

	tmxparser::TmxTileset tileset = _map->tilesetCollection[0];
	ASSERT_EQ(1, tileset.firstgid);
	ASSERT_EQ("super_mario_one_tileset", tileset.name);
	ASSERT_EQ(16, tileset.tileWidth);
	ASSERT_EQ(16, tileset.tileHeight);

	tileset = _map->tilesetCollection[1];
	ASSERT_EQ(925, tileset.firstgid);
	ASSERT_EQ("legend_of_zelda_one_overworld_tiles", tileset.name);
	ASSERT_EQ(16, tileset.tileWidth);
	ASSERT_EQ(16, tileset.tileHeight);
}


TEST_F(TmxParseTest, TilesetImageSourceProperties)
{
	ASSERT_EQ(2, _map->tilesetCollection.size());

	tmxparser::TmxTileset tileset = _map->tilesetCollection[0];
	tmxparser::TmxImage image = tileset.image;

	ASSERT_EQ("assets/textures/super_mario_one_tileset.png", image.source);
	ASSERT_EQ(528, image.width);
	ASSERT_EQ(448, image.height);

	tileset = _map->tilesetCollection[1];
	image = tileset.image;

	ASSERT_EQ("assets/textures/legend_of_zelda_one_overworld_tiles.png", image.source);
	ASSERT_EQ(307, image.width);
	ASSERT_EQ(163, image.height);
}


TEST_F(TmxParseTest, LayerProperties)
{
	ASSERT_EQ(1, _map->layerCollection.size());
	tmxparser::TmxLayer layer = _map->layerCollection[0];

	ASSERT_EQ("World", layer.name);
	ASSERT_EQ(10, layer.width);
	ASSERT_EQ(10, layer.height);

	ASSERT_EQ(100, layer.tiles.size());
}


TEST_F(TmxParseTest, TilesValidation)
{
	ASSERT_EQ(1, _map->layerCollection.size());
	tmxparser::TmxLayer layer = _map->layerCollection[0];

	tmxparser::TmxLayerTileCollection_t tiles = layer.tiles;
	ASSERT_EQ(100, tiles.size());

	int expectedGidValues[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
							34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
							925, 926, 927, 928, 929, 930, 931, 932, 933, 934
							};
	int testTileCount = sizeof(expectedGidValues) / sizeof(int);
	for (unsigned int i = 0; i < testTileCount; i++)
	{
		ASSERT_EQ(expectedGidValues[i], tiles[i].gid);
	}

	int remaining = tiles.size() - testTileCount;
	for (unsigned int i = testTileCount; i < remaining; i++)
	{
		ASSERT_EQ(0, tiles[i].gid);
	}
}


int main(int argc, char **argv)
{
	int retVal = 0;

	::testing::InitGoogleTest(&argc, argv);

	printf("\033[32m" "==========\nXML TESTS\n==========\n" "\033[0m");
	retVal = RUN_ALL_TESTS();

	//printf("\033[32m" "==========\nCSV TESTS\n==========\n" "\033[0m");
	//retVal = RUN_ALL_TESTS();

	//printf("\033[32m" "==========\nBASE64 TESTS\n==========\n" "\033[0m");
	//retVal = RUN_ALL_TESTS();

	return retVal;
}
