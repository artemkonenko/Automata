cmake_minimum_required ( VERSION 2.6 )
set ( CMAKE_CXX_FLAGS "-std=c++11" )

project ( Automata )

set( CMAKE_BUILD_TYPE RELEASE )
set ( APP_NAME automata )


include_directories ( ${PROJECT_SOURCE_DIR}/include )

set ( SRC_DIR ${PROJECT_SOURCE_DIR}/src )
list(APPEND SRC ${SRC_DIR}/main.cpp )
list(APPEND SRC ${SRC_DIR}/automata.cpp )

set ( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )
add_executable ( ${APP_NAME} ${SRC} )