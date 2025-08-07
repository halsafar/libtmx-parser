# libtmx-parser

## Tested Operating Systems
- Debian 12
- Ubuntu 14.04
- Android 2.2+/3.X/4.X


## Features
- Parses tiled-qt 0.10.2 maps, version 1.0+ tmx files
- Simple, returns a struct filled with map data
- Lightweight
- Using TinyXML2
- Parses XML, CSV and compressed or uncompressed Base64 layers
- Easy to drop into a project


# TODO
- Unit/Coverage tests
- Parse terrain
- Check for NULLs and report errors on certain 'required' attributes


# COMPILING


- See Makefile for an example


## Requires libs
tinyxml2 is included as a subrepo.  You can get it by running the following in this cloned repo:
- git submodule update --init --recursive
- [TinyXml2](https://github.com/leethomason/tinyxml2) is being used, as it is very lightweight and game dev friendly

zlib and zstandard are required as external libraries.


## Required files
- tmxparser.h/.cpp
- base64.h/cpp
- tinyxml2.h/.cpp
- compression.h/cpp


#USAGE
```Cpp
tmxparser::TmxMap map;
tmxparser::TmxReturn error = tmxparser::parseFromFile("example.tmx", &map);
```
