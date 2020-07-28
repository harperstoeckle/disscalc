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

option(
	DISSCALC_ENABLE_WARNINGS
	"Enable maximal warnings for dissonance calculator"
	ON
)
if (DISSCALC_ENABLE_WARNINGS)
	set(MSVC_WARNINGS /W4)
	set(GCC_WARNINGS -pedantic -Wall -Wextra -Wcast-align -Wcast-qual
		-Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2
		-Winit-self -Wlogical-op -Wmissing-declarations
		-Wmissing-include-dirs -Wnoexcept -Wold-style-cast
		-Woverloaded-virtual -Wredundant-decls -Wshadow
		-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel
		-Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
		-Wimplicit-fallthrough=5 -Wswitch-enum
	)

	target_compile_options(disscalc-internal
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>
		$<$<CXX_COMPILER_ID:GNU,Clang>:${GCC_WARNINGS}>
	)
	target_compile_options(disscalc
		PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>
		$<$<CXX_COMPILER_ID:GNU,Clang>:${GCC_WARNINGS}>
	)

	unset(GCC_WARNINGS)
	unset(MSVC_WARNINGS)
endif()
