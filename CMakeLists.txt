cmake_minimum_required(VERSION 3.10.2)
project(gdbd)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

set(CMAKE_CXX_FLAGS -pthread)

file(GLOB SLAVE_SOURCE_FILES
        "source/SQLite/*.h")


add_executable(cliente source/cliente.cpp)
add_executable(servidor source/servidor.cpp)
add_executable(nodos source/nodos.cpp)
add_executable(nodo2 source/nodo2.cpp)
add_executable(nodo3 source/nodo3.cpp)

find_package (SQLite3)
if (SQLITE3_FOUND)
    include_directories(${SQLITE3_INCLUDE_DIRS})
    target_link_libraries (nodos sqlite3${SQLITE3_LIBRARIES})
    target_link_libraries (nodo2 sqlite3${SQLITE3_LIBRARIES})
    target_link_libraries (nodo3 sqlite3${SQLITE3_LIBRARIES})
endif (SQLITE3_FOUND)
#add_executable(pruebas source/pruebas.cpp)

# add_executable(pruebas source/pruebas.cpp)
#add_executable(pruebas source/pruebas.cpp)