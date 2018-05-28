#include "gtest/gtest.h"
#include "../src/tmxparser.h"


/*template<>
bool std::operator==(const tmxparser::TmxShapePoint& l, const tmxparser::TmxShapePoint& r)
{
    return (l.first == r.first && l.second == r.second) ||
           (l.first == r.second && l.second == r.first);
}*/


class TmxParseTest : public ::testing::Test
{
public:
	static void LoadMap(const std::string& mapPath)
	{
		if (_map != NULL)
		{
			delete _map;
		}

		_map = new tmxparser::TmxMap();
		tmxparser::parseFromFile(mapPath, _map, "../test_files");
	}


protected:
	static void SetUpTestCase()
	{
		if (_map == NULL)
		{
			fprintf(stderr, "TEST FIXTURE NOT SETUP PROPERLY!!!\n");
		}
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
	ASSERT_EQ(0, _map->orientation);
	ASSERT_EQ("right-down", _map->renderOrder);
	ASSERT_EQ(10, _map->width);
	ASSERT_EQ(10, _map->height);
	ASSERT_EQ(16, _map->tileWidth);
	ASSERT_EQ(16, _map->tileHeight);
}


TEST_F(TmxParseTest, TilesetProperties)
{
	ASSERT_EQ(3, _map->tilesetCollection.size());

	tmxparser::TmxTileset tileset = _map->tilesetCollection[0];
	ASSERT_EQ(1, tileset.firstgid);
	ASSERT_EQ("super_mario_one_tileset", tileset.name);
	ASSERT_EQ(16, tileset.tileWidth);
	ASSERT_EQ(16, tileset.tileHeight);

	tileset = _map->tilesetCollection[1];
	ASSERT_EQ(925, tileset.firstgid);
	ASSERT_EQ("super_mario_one_tileset", tileset.name);
	ASSERT_EQ(16, tileset.tileWidth);
	ASSERT_EQ(16, tileset.tileHeight);

	tileset = _map->tilesetCollection[2];
	ASSERT_EQ(1621, tileset.firstgid);
	ASSERT_EQ("legend_of_zelda_one_overworld_tiles", tileset.name);
	ASSERT_EQ(16, tileset.tileWidth);
	ASSERT_EQ(16, tileset.tileHeight);
}


TEST_F(TmxParseTest, TileDefinitions)
{
	ASSERT_EQ(3, _map->tilesetCollection.size());

	tmxparser::TmxTileset tileset = _map->tilesetCollection[0];

	ASSERT_EQ(2, tileset.tileDefinitions.size());

	tmxparser::TmxTileDefinition def = tileset.tileDefinitions[3];

	ASSERT_EQ(3, def.id);
	ASSERT_EQ(1, def.objectgroups.size());

	tmxparser::TmxObjectGroup objGrp = def.objectgroups[0];

	ASSERT_EQ(1, objGrp.objects.size());
	ASSERT_EQ(3.18182f, objGrp.objects[0].x);
	ASSERT_EQ(3.63636f, objGrp.objects[0].width);
	ASSERT_EQ(4.72727f, objGrp.objects[0].height);
	ASSERT_EQ(3, objGrp.objects[0].shapeType);
	ASSERT_EQ(0, objGrp.objects[0].rotation);
	ASSERT_EQ(0, objGrp.objects[0].visible);
	ASSERT_EQ(0, objGrp.objects[0].referenceGid);
}


TEST_F(TmxParseTest, TilesetImageSourceProperties)
{
	ASSERT_EQ(3, _map->tilesetCollection.size());

	tmxparser::TmxTileset tileset = _map->tilesetCollection[0];
	tmxparser::TmxImage image = tileset.image;

	ASSERT_EQ("../test_files/super_mario_one_tileset.png", image.source);
	ASSERT_EQ(528, image.width);
	ASSERT_EQ(448, image.height);

	tileset = _map->tilesetCollection[1];
	image = tileset.image;

	ASSERT_EQ("../test_files/super_mario_one_tileset.png", image.source);
	ASSERT_EQ(528, image.width);
	ASSERT_EQ(448, image.height);

	tileset = _map->tilesetCollection[2];
	image = tileset.image;

	ASSERT_EQ("../test_files/legend_of_zelda_one_overworld_tiles.png", image.source);
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
							1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630,
							1639, 1640, 1641, 1642, 1643, 1644, 1645, 1646, 1647, 1648,
							61};
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


TEST_F(TmxParseTest, ObjectGroupValidation)
{
	ASSERT_EQ(1, _map->objectGroupCollection.size());

	tmxparser::TmxObjectGroup objGroup = _map->objectGroupCollection[0];

	ASSERT_EQ("Collision", objGroup.name);
}


TEST_F(TmxParseTest, ObjectsValidation)
{
	ASSERT_EQ(1, _map->objectGroupCollection.size());
	tmxparser::TmxObjectGroup objGroup = _map->objectGroupCollection[0];

	ASSERT_EQ(4, objGroup.objects.size());

	tmxparser::TmxObject obj = objGroup.objects[0];
	ASSERT_EQ("testRect", obj.name);
	ASSERT_EQ("testRect", obj.type);
	ASSERT_EQ(0, obj.x);
	ASSERT_EQ(0, obj.y);
	ASSERT_EQ(160, obj.width);
	ASSERT_EQ(160, obj.height);
	ASSERT_EQ(tmxparser::kSquare, obj.shapeType);

	obj = objGroup.objects[1];
	ASSERT_EQ("testCircle", obj.name);
	ASSERT_EQ("testType", obj.type);
	ASSERT_EQ(48, obj.x);
	ASSERT_EQ(83, obj.y);
	ASSERT_EQ(73, obj.width);
	ASSERT_EQ(73, obj.height);
	ASSERT_EQ(tmxparser::kEllipse, obj.shapeType);

	obj = objGroup.objects[2];
	ASSERT_EQ("testPolygon", obj.name);
	ASSERT_EQ("testPolygon", obj.type);
	ASSERT_EQ(134, obj.x);
	ASSERT_EQ(73, obj.y);
	ASSERT_EQ(tmxparser::kPolygon, obj.shapeType);

	ASSERT_EQ(4, obj.shapePoints.size());
	ASSERT_EQ(tmxparser::TmxShapePoint(0,0), obj.shapePoints[0]);
	ASSERT_EQ(tmxparser::TmxShapePoint(8,59), obj.shapePoints[1]);
	ASSERT_EQ(tmxparser::TmxShapePoint(-106,59), obj.shapePoints[2]);
	ASSERT_EQ(tmxparser::TmxShapePoint(-79,-10), obj.shapePoints[3]);

	obj = objGroup.objects[3];
	ASSERT_EQ("testPolyline", obj.name);
	ASSERT_EQ("testPolyline", obj.type);
	ASSERT_EQ(15.5, obj.x);
	ASSERT_EQ(69.5, obj.y);
	ASSERT_EQ(tmxparser::kPolyline, obj.shapeType);

	ASSERT_EQ(6, obj.shapePoints.size());
	ASSERT_EQ(tmxparser::TmxShapePoint(0,0), obj.shapePoints[0]);
	ASSERT_EQ(tmxparser::TmxShapePoint(7,25.5), obj.shapePoints[1]);
	ASSERT_EQ(tmxparser::TmxShapePoint(23.25, 14.75), obj.shapePoints[2]);
	ASSERT_EQ(tmxparser::TmxShapePoint(21, -6), obj.shapePoints[3]);
	ASSERT_EQ(tmxparser::TmxShapePoint(1, -11.25), obj.shapePoints[4]);
	ASSERT_EQ(tmxparser::TmxShapePoint(0.136364, 0.318182), obj.shapePoints[5]);
}


int main(int argc, char **argv)
{
	int retVal = 0;

	::testing::InitGoogleTest(&argc, argv);

	printf("\033[32m" "==========\nXML TESTS\n==========\n" "\033[0m");
	TmxParseTest::LoadMap("../test_files/test_xml_level.tmx");
	retVal = RUN_ALL_TESTS();

	printf("\033[32m" "==========\nEXRERNAL TILESET TESTS\n==========\n" "\033[0m");
	TmxParseTest::LoadMap("../test_files/test_xml_level_ext_tileset.tmx");
	retVal = RUN_ALL_TESTS();

	printf("\033[32m" "==========\nCSV TESTS\n==========\n" "\033[0m");
	TmxParseTest::LoadMap("../test_files/test_csv_level.tmx");
	retVal = !retVal && RUN_ALL_TESTS();

	printf("\033[32m" "==========\nBASE64 TESTS\n==========\n" "\033[0m");
	TmxParseTest::LoadMap("../test_files/test_base64_level.tmx");
	retVal = !retVal && RUN_ALL_TESTS();

	return retVal;
}
