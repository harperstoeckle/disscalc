function(enable_compile_warnings target)
	string(TOUPPER "${target}" TARGET_UPPERCASE)
	# Name of the option used to enable or disable warnings.
	set(TARGET_WARNING_OPTION_NAME "${TARGET_UPPERCASE}_ENABLE_WARNINGS")

	option(
		${TARGET_WARNING_OPTION_NAME}
		"Enable maximal warnings for ${target}"
		ON
	)
	if (${TARGET_WARNING_OPTION_NAME})
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

		target_compile_options("${target}"
			PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNINGS}>
			$<$<CXX_COMPILER_ID:GNU,Clang>:${GCC_WARNINGS}>
		)
	endif()
endfunction()
