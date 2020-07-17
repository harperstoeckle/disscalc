add_executable(disscalc-tests
	main.test.cpp
	command-line.test.cpp
)
target_link_libraries(disscalc-tests
	PRIVATE
	disscalc-internal
)
target_include_directories(disscalc-tests
	PRIVATE
	${PROJECT_SOURCE_DIR}/src
)

find_package(Catch2)
target_link_libraries(disscalc-tests
	PRIVATE
	Catch2::Catch2
)
