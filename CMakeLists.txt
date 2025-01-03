cmake_minimum_required(VERSION 3.29)
project(NewJS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

execute_process(
        COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --system-libs --libnames
        OUTPUT_VARIABLE LLVM_LIBNAMES
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
separate_arguments(LLVM_LIBNAMES_LIST NATIVE_COMMAND ${LLVM_LIBNAMES})
separate_arguments(LLVM_DEFS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

message(STATUS "LLVM version ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM config ${LLVM_DIR}")
message(STATUS "LLVM definitions ${LLVM_DEFS_LIST}")
message(STATUS "LLVM libraries ${LLVM_LIBNAMES_LIST}")
message(STATUS "LLVM lib dirs ${LLVM_LIBRARY_DIRS}")

file(GLOB_RECURSE sources "src/*.cpp" "include/*.hpp")
add_executable(NewJS ${sources})
target_compile_definitions(NewJS PUBLIC ${LLVM_DEFS_LIST})
target_include_directories(NewJS PUBLIC "include" ${LLVM_INCLUDE_DIRS})
target_link_directories(NewJS PUBLIC ${LLVM_LIBRARY_DIRS})
target_link_libraries(NewJS PUBLIC ${LLVM_LIBNAMES_LIST})

file(GLOB_RECURSE lib_sources "lib/*.cpp")
add_library(StdNewJS ${lib_sources})
target_include_directories(StdNewJS PUBLIC "include")

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET NewJS POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:NewJS> $<TARGET_FILE:NewJS>.dbg
            COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:NewJS>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:NewJS>.dbg $<TARGET_FILE:NewJS>
    )
endif ()

install(TARGETS NewJS)
install(TARGETS StdNewJS)
