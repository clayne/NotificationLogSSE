find_package(
	Python3
	3.10
	MODULE
	REQUIRED
	COMPONENTS
		Interpreter
)

set_root_directory()

set(PAPYRUS_SOURCE_DIR "${ROOT_DIR}/src/papyrus")
set(PAPYRUS_SOURCES
	"${PAPYRUS_SOURCE_DIR}/NotificationLog_MCM.psc"
	"${PAPYRUS_SOURCE_DIR}/NotificationLog.psc"
)

set(TOOL_SOURCE "${ROOT_DIR}/tools/papyrus_compile.py")

source_group("papyrus" FILES ${PAPYRUS_SOURCES})
source_group("python" FILES "${TOOL_SOURCE}")

set(COMMAND_ARGS
	"$<TARGET_FILE:Python3::Interpreter>"
	"${TOOL_SOURCE}"
	"--game-dir=${Skyrim64Path}"
	"--skyui-dir=${ROOT_DIR}/external/skyui/dist/Data/Scripts/Headers"
	"--source-dir=${ROOT_DIR}/src/papyrus"
)

if("${COPY_OUTPUT}")
	list(APPEND COMMAND_ARGS "--copy-build")
endif()

foreach(ELEMENT ${PAPYRUS_SOURCES})
	cmake_path(GET ELEMENT STEM ELEMENT)
	list(APPEND PAPYRUS_COMPILED "${CMAKE_CURRENT_BINARY_DIR}/artifacts/${ELEMENT}.pex")
endforeach()

add_custom_target(
	"papyrus_compile"
	COMMAND
		${COMMAND_ARGS}
	DEPENDS
	BYPRODUCTS
		${PAPYRUS_COMPILED}
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	VERBATIM
	SOURCES
		${PAPYRUS_SOURCES}
		"${TOOL_SOURCE}"
)

install(
	FILES
		${PAPYRUS_COMPILED}
	DESTINATION "Scripts"
	COMPONENT "main"
)
