cmake_minimum_required(VERSION 3.29)
project(NewJS)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

message(STATUS "LLVM version ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM config ${LLVM_DIR}")

execute_process(
        COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --libs
        OUTPUT_VARIABLE LLVM_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LLVM_LIBS "${LLVM_LIBS} -lws2_32")
endif ()

message(STATUS "LLVM libraries ${LLVM_LIBS}")
message(STATUS "LLVM definitions ${LLVM_DEFINITIONS}")

file(GLOB_RECURSE sources "src/*.cpp" "include/*.hpp")
add_executable(NewJS ${sources})
target_compile_definitions(NewJS PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(NewJS PUBLIC "include" ${LLVM_INCLUDE_DIRS})
target_link_directories(NewJS PUBLIC ${LLVM_LIBRARY_DIR})
target_link_libraries(NewJS PUBLIC ${LLVM_LIBS})
