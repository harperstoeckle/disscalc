# Other than main, convert the sources into a static library for easy testing.
add_library(disscalc-internal STATIC
	disscalc/args-parsing.cpp disscalc/args-parsing.hpp
	disscalc/options.cpp disscalc/options.hpp
	disscalc/dissonance.cpp disscalc/dissonance.hpp
	disscalc/output.cpp disscalc/output.hpp
	disscalc/table.hpp
	${CMAKE_CURRENT_BINARY_DIR}/generated/usage.hpp
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

# Configured files are under ${PROJECT_BINARY_DIR}/src (in generated).
target_include_directories(disscalc-internal PUBLIC ${PROJECT_BINARY_DIR}/src)

# Regenerate usage.hpp when usage.txt changes.
set_property(
	DIRECTORY APPEND
	PROPERTY
	CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generated/usage.txt
)

file(READ generated/usage.txt usage_message_content)
configure_file(generated/usage.hpp.in generated/usage.hpp @ONLY)

add_executable(disscalc main.cpp)

target_link_libraries(disscalc
	PRIVATE
	disscalc-internal
)

enable_compile_warnings(disscalc-internal)
enable_compile_warnings(disscalc)
