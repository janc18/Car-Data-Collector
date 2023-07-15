cmake_minimum_required(VERSION 3.12)
project(Car-data-collector C)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK3 REQUIRED gtk+-3.0)
include_directories (include src ${GTK3_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
file(GLOB MAIN
	"src/*.c"
	"include/*.h"
)
link_directories (${GTK3_LIBRARY_DIRS})

add_definitions (${GTK3_CFLAGS_OTHER})

add_executable(CarDataCollector src/main.c "${MAIN}")

set_target_properties(CarDataCollector PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries (CarDataCollector ${GTK3_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
