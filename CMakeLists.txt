cmake_minimum_required(VERSION 3.21)

set( PROJECT_NAME "Construction-Summary-Engine" )
project( ${PROJECT_NAME} LANGUAGES CXX )
set( TARGET ${PROJECT_NAME} )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# Set source and include paths
set( INCLUDE_PATH "${PROJECT_SOURCE_DIR}/include/" )
set( SOURCE_PATH "${PROJECT_SOURCE_DIR}/src/" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Recursively find files
file( GLOB_RECURSE HPPS "${INCLUDE_PATH}/*.hpp" )
file( GLOB_RECURSE INLS "${INCLUDE_PATH}/*.inl" )
file( GLOB_RECURSE CPPS "${SOURCE_PATH}/*.cpp"  )

add_executable( ${TARGET} ${CPPS} ${INLS} ${HPPS} )

# Include header file directory to target
target_include_directories( ${TARGET} PUBLIC $<BUILD_INTERFACE:${INCLUDE_PATH}> $<INSTALL_INTERFACE:include/> )

target_link_libraries( ${TARGET} PUBLIC )

install(TARGETS ${TARGET}
        RUNTIME DESTINATION)