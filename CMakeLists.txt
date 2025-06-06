cmake_minimum_required(VERSION 3.20)
project(NewJS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

execute_process(
        COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --system-libs --libnames core support passes all-targets
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

file(GLOB_RECURSE includes include/*.hpp)

file(GLOB_RECURSE common_sources src/common/*.cpp)
add_library(common ${common_sources} ${includes})
target_include_directories(common PUBLIC include ${LLVM_INCLUDE_DIRS})
target_compile_definitions(common PUBLIC ${LLVM_DEFS_LIST})
target_link_directories(common PUBLIC ${LLVM_LIBRARY_DIRS})
target_link_libraries(common PUBLIC ${LLVM_LIBNAMES_LIST})

file(GLOB_RECURSE cli_sources src/cli/*.cpp)
add_executable(newjs ${cli_sources} ${includes})
target_include_directories(newjs PRIVATE include)
target_link_libraries(newjs PRIVATE common)

file(GLOB_RECURSE standard_sources src/standard/*.cpp)
add_library(stdnewjs ${standard_sources} ${includes})
target_include_directories(stdnewjs PUBLIC include)
target_compile_options(stdnewjs PRIVATE -fno-rtti -fno-exceptions)

install(TARGETS newjs)
install(TARGETS stdnewjs)

if (NOT MSVC)
    if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
        add_custom_command(TARGET newjs POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:newjs> $<TARGET_FILE:newjs>.dbg
                COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:newjs>
                COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:newjs>.dbg $<TARGET_FILE:newjs>
                COMMENT "Separating debug information from executable"
        )
        install(FILES $<TARGET_FILE:newjs>.dbg DESTINATION bin)
    endif ()
else ()
    if (CMAKE_BUILD_TYPE STREQUAL Debug OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
        install(FILES ${CMAKE_BINARY_DIR}/newjs.ilk ${CMAKE_BINARY_DIR}/newjs.pdb DESTINATION bin)
    endif ()
endif ()
