cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(spectralyze)

add_executable(spectralyze
	"src/main.cpp"
 "src/FFT.hpp"
 )

target_include_directories(spectralyze PRIVATE
	"lib/AudioFile"
	"lib/json"
	"lib/cxxopts"
)