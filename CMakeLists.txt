cmake_minimum_required(VERSION 3.15...3.30)

# Project Definition
project(CppLearning
    VERSION 1.0.0
    DESCRIPTION "A standardized production-ready repository for learning advanced C++ paradigms"
    LANGUAGES CXX
)

# Force Modern C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define Global Include Paths (Makes common.hpp accessible everywhere)
include_directories(src/include)

# Enforce Production-Grade Compiler Warning Levels
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
        -O2                 # Standard production optimization level
    )
elseif(MSVC)
    add_compile_options(/W4 /WX /O2)
endif()

# Output Configuration (Directs all binaries strictly inside the build/ directory)

#[[
    ${CMAKE_BINARY_DIR}: This is a dynamic variable that points to the exact folder where 
    you ran the cmake .. command. In your workflow, this points directly to your build/ folder.
]]

#[[ 
    CMAKE_RUNTIME_OUTPUT_DIRECTORY: This tells the compiler, 
    "No matter which sub-folder or chapter a C++ file belongs to, when you finish 
    compiling a standalone executable binary (a runtime program), route it straight 
    to this target destination." By setting it to ${CMAKE_BINARY_DIR}/bin, 
    everything routes to build/bin/.
]]
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#[[ 
    CMAKE_LIBRARY_OUTPUT_DIRECTORY & CMAKE_ARCHIVE_OUTPUT_DIRECTORY: 
    These do the exact same thing but for code libraries (like .so, .a, or .lib files) if 
    you ever decide to split your code into reusable modules rather than standalone programs. 
    They route cleanly to build/lib/.
]]

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Process Subdirectories Sequentially
add_subdirectory(src)
add_subdirectory(tests)

# For documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    # This creates a 'doc_doxygen' target
    doxygen_add_docs(
        doc_doxygen
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ALL
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()