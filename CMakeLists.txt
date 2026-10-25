cmake_minimum_required(VERSION 4.0)
project(CSGO-SDK)

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY /game/bin )

# 3rdparty
add_subdirectory( SDL )
include_directories( SDL/include )

# sdk
include_directories( src )
add_subdirectory( src/tier1 )
