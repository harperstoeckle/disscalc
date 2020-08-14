# Other than main, convert the sources into a static library for easy testing.
add_library(disscalc-internal STATIC
	disscalc/args-parsing.cpp disscalc/args-parsing.hpp
	disscalc/options.cpp disscalc/options.hpp
	disscalc/dissonance.cpp disscalc/dissonance.hpp
	disscalc/output.cpp disscalc/output.hpp
	disscalc/table.hpp
)
set_target_properties(disscalc-internal
	PROPERTIES
	CXX_EXTENSIONS OFF
)
target_compile_features(disscalc-internal
	PUBLIC
	cxx_std_20
)
target_include_directories(disscalc-internal
	PUBLIC
	${PROJECT_SOURCE_DIR}/src
)

add_executable(disscalc main.cpp)

target_link_libraries(disscalc
	PRIVATE
	disscalc-internal
)

enable_compile_warnings(disscalc-internal)
enable_compile_warnings(disscalc)
