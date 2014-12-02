#include "gtest/gtest.h"
#include "../src/tmxparser.h"

class TmxParseTest : public ::testing::Test
{
protected:
	static void SetUpTestCase()
	{
		_map = new tmxparser::TmxMap();
		tmxparser::parseFromFile("../test_xml_level.tmx", _map, "assets/textures/");
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
	ASSERT_EQ(16, _map->width);
	ASSERT_EQ(16, _map->height);
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
	ASSERT_EQ("BowserBreath", tileset.name);
	ASSERT_EQ(32, tileset.tileWidth);
	ASSERT_EQ(32, tileset.tileHeight);
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

	ASSERT_EQ("assets/textures/BowserBreath.png", image.source);
	ASSERT_EQ(128, image.width);
	ASSERT_EQ(32, image.height);
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
