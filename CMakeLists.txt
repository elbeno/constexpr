cmake_minimum_required (VERSION 3.0.2)
project (constexpr)
include_directories ("${PROJECT_SOURCE_DIR}/src/include")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(MY_CXX_FLAGS "-pedantic \
-ffunction-sections \
-Wall \
-Wextra \
-Wcast-align \
-Wcast-qual \
-Wctor-dtor-privacy \
-Wdisabled-optimization \
-Wformat=2 \
-Winit-self \
-Wmissing-include-dirs \
-Wno-undefined-inline \
-Wno-undefined-internal \
-Wold-style-cast \
-Woverloaded-virtual \
-Wredundant-decls \
-Wshadow \
-Wsign-conversion \
-Wsign-promo \
-Wstrict-overflow=5 \
-Wswitch-default \
-Wundef \
-Werror")

option(USE_CLANG "build application with clang" ON)

if (USE_CLANG)
    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS    "${MY_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
else ()
    SET (CMAKE_CXX_FLAGS    "${MY_CXX_FLAGS} -std=c++14")
endif ()

add_subdirectory (src/test)
