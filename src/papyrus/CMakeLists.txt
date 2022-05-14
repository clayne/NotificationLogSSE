find_package(
	Python3
	3.10
	MODULE
	REQUIRED
	COMPONENTS
		Interpreter
)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
cmake_path(NORMAL_PATH ROOT_DIR)
if("${ROOT_DIR}" MATCHES "[\\/]$")
	string(LENGTH "${ROOT_DIR}" ROOT_DIR_LENGTH)
	math(EXPR ROOT_DIR_LENGTH "${ROOT_DIR_LENGTH} - 1")
	string(SUBSTRING "${ROOT_DIR}" 0 "${ROOT_DIR_LENGTH}" ROOT_DIR)
endif()

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

add_custom_target(
	"papyrus_compile"
	COMMAND
		${COMMAND_ARGS}
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	VERBATIM
	SOURCES
		${PAPYRUS_SOURCES}
		"${TOOL_SOURCE}"
)

foreach(ELEMENT ${PAPYRUS_SOURCES})
	cmake_path(GET ELEMENT STEM ELEMENT)
	list(APPEND PAPYRUS_COMPILED "${CMAKE_CURRENT_BINARY_DIR}/artifacts/${ELEMENT}.pex")
endforeach()

install(
	FILES
		${PAPYRUS_COMPILED}
	DESTINATION "Scripts"
	COMPONENT "main"
)
