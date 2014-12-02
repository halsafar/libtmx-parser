#include "gtest/gtest.h"
#include "../src/tmxparser.h"

class TmxParseTest : public ::testing::Test {
 protected:
  virtual void SetUp()
  {
	  _map = new tmxparser::TmxMap();
	  tmxparser::parseFromFile("../test_xml_level.tmx", _map, "assets/textures/");
  }

  // virtual void TearDown() {}

  tmxparser::TmxMap* _map;
};


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
	//ASSERT_EQ(16, _map->nextObjectId);
}


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
