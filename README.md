libtmx-parser
=============

Using TinyXML2 and designed to drop into any codebase.

TODO
=============
- Parse objects
- Parse terrain
- Possibly remove c++11 requirement


COMPILING
=============


USAGE
=============

tmxparser::TmxMap map;
tmxparser::TmxReturn error = tmxparser::parseFromFile("example.tmx", &map);
