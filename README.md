# libtmx-parser

## Features
- Parses tiled-qt 0.9.1 maps, version 1.0 tmx files
- Simple, returns a struct filled with map data
- Lightweight
- Using TinyXML2
- Easy to drop into a project


# TODO
- Parse objects
- Parse terrain
- Remove c++11 requirement
- Go C


# COMPILING
## Required files
- tmxparser.h
- tmxparser.cpp
- tinyxml2.h
- tinyxml2.cpp

## Required flags
```
-std=c++11
```


#USAGE
```Cpp
tmxparser::TmxMap map;
tmxparser::TmxReturn error = tmxparser::parseFromFile("example.tmx", &map);
```
