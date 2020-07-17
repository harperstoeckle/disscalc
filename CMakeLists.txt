# 3.12 required for cxx_std_20
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(disscalc
	VERSION 0.0.0
	LANGUAGES CXX
)

add_subdirectory(src)

option(
	DISSCALC_BUILD_TESTS
	"Build tests for dissonance calculator"
	OFF
)
if (DISSCALC_BUILD_TESTS)
	add_subdirectory(tests)
endif()
