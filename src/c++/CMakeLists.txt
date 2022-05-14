set_root_directory()

set(SOURCE_DIR "${ROOT_DIR}/src/c++")
set(SOURCE_FILES
	"${SOURCE_DIR}/main.cpp"
	"${SOURCE_DIR}/NotificationLogger.h"
	"${SOURCE_DIR}/PCH.h"
)

source_group("src" FILES ${SOURCE_FILES})

set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"${ROOT_DIR}/cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)

source_group("src" FILES "${VERSION_HEADER}")

configure_file(
	"${ROOT_DIR}/cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCE_FILES}
	"${VERSION_HEADER}"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	"${ROOT_DIR}/.clang-format"
	"${ROOT_DIR}/.editorconfig"
)

target_compile_features(
	"${PROJECT_NAME}"
	PRIVATE
		cxx_std_20
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		"${PROJECT_NAME}"
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		"${PROJECT_NAME}"
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
		"${CMAKE_CURRENT_BINARY_DIR}/src"
		"${SOURCE_DIR}"
)

add_subdirectory("${ROOT_DIR}/external/CommonLibSSE" CommonLibSSE EXCLUDE_FROM_ALL)
find_package(spdlog REQUIRED CONFIG)

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
		"${SOURCE_DIR}/PCH.h"
)

install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
	COMPONENT "main"
)

install(
	FILES
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "/"
	COMPONENT "pdbs"
)

if("${COPY_OUTPUT}")
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${Skyrim64Path}/Data/SKSE/Plugins/"
		COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${Skyrim64Path}/Data/SKSE/Plugins/"
		VERBATIM
	)
endif()

macro(handle_data)
	set(_PREFIX handle_data)

	set(_OPTIONS)
	set(_ONE_VALUE_ARGS
		DESTINATION
	)
	set(_MULTI_VALUE_ARGS
		FILES
	)

	set(_REQUIRED
		FILES
	)

	cmake_parse_arguments(
		"${_PREFIX}"
		"${_OPTIONS}"
		"${_ONE_VALUE_ARGS}"
		"${_MULTI_VALUE_ARGS}"
		"${ARGN}"
	)

	foreach(_ARG ${_REQUIRED})
		if(NOT DEFINED "${_PREFIX}_${_ARG}")
			message(FATAL_ERROR "Argument is required to be defined: ${_ARG}")
		endif()
	endforeach()

	if(DEFINED ${_PREFIX}_DESTINATION)
		set(${_PREFIX}_INSTALL_DESTINATION "${${_PREFIX}_DESTINATION}")
		set(${_PREFIX}_COPY_DESTINATION "${${_PREFIX}_DESTINATION}")
	else()
		set(${_PREFIX}_INSTALL_DESTINATION "/")
		set(${_PREFIX}_COPY_DESTINATION "")
	endif()

	install(
		FILES ${${_PREFIX}_FILES}
		DESTINATION "${${_PREFIX}_INSTALL_DESTINATION}"
		COMPONENT "main"
	)

	if("${COPY_OUTPUT}")
		foreach(_FILE ${${_PREFIX}_FILES})
			add_custom_command(
				TARGET "${PROJECT_NAME}"
				POST_BUILD
				COMMAND
					"${CMAKE_COMMAND}"
					-E
					copy_if_different
					"${_FILE}"
					"${Skyrim64Path}/Data/${${_PREFIX}_COPY_DESTINATION}"
				VERBATIM
			)
		endforeach()
	endif()
endmacro()

set(TRANSLATIONS_DIR "${ROOT_DIR}/data/Data/Interface/Translations")
handle_data(
	FILES
		"${TRANSLATIONS_DIR}/NotificationLog_english.txt"
	DESTINATION "Interface/Translations"
)

set(DATA_DIR "${ROOT_DIR}/data/Data")
handle_data(
	FILES
		"${DATA_DIR}/NotificationLog.esp"
)
