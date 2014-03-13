# libtmx-parser

## Features
- Parses tiled 0.9.1 maps.
- Lightweight
- Using TinyXML2
- Easy to drop into a project


# TODO
- Parse objects
- Parse terrain
- Possibly remove c++11 requirement


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
