cmake_minimum_required(VERSION 3.12)
project(CAE32_IG C)
option(DEBUG "Enable debug build" OFF)
if(DEBUG)
    message("Compiling with debug flags")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
else()
    message("Compiling without debug flags")
endif()
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)
include_directories (include src ${GTK3_INCLUDE_DIRS})
file(GLOB MAIN
	"src/*.c"
	"include/*.h"
)
link_directories (${GTK3_LIBRARY_DIRS})

add_definitions (${GTK3_CFLAGS_OTHER})

add_executable(executable main.c "${MAIN}")

set_target_properties(executable PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries (executable ${GTK3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} m)
