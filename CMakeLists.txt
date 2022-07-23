cmake_minimum_required(VERSION 3.10.2)
project(gdbd)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

set(CMAKE_CXX_FLAGS -pthread)

file(GLOB SLAVE_SOURCE_FILES
        "source/SQLite/*.h")


add_executable(client source/client.cpp)
add_executable(server source/server.cpp)
add_executable(node source/node.cpp)


find_package (SQLite3)
if (SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIRS})
    target_link_libraries (node sqlite3${SQLITE3_LIBRARIES})

endif (SQLITE3_FOUND)
#add_executable(pruebas source/pruebas.cpp)
