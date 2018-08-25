cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME deltaHQ)

set(CMAKE_BUILD_TYPE DEBUG)

project(${PROJECT_NAME} C)

if(UNIX)
set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE string "clang compiler" FORCE)
endif(UNIX)
if(APPLE)
set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang" CACHE string "clang compiler" FORCE)
endif(APPLE)

set(CMAKE_C_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING")

set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -pg -fno-omit-frame-pointer -fsanitize=address,leak")
set(CMAKE_C_FLAGS_RELEASE "-O2 -s -DNODEBUG -DMESSAGE_WARNING")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os -s -DNODEBUG")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)

include_directories(./include)

set(libs m c pthread expat jansson)

set(sources_dir deltaHQ)
set(headers_dir include)
file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} ${sources_dir}/*.c)
file(GLOB headers RELATIVE ${PROJECT_SOURCE_DIR} ${headers_dir}/*.h)


add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE ./include)
target_link_libraries(${PROJECT_NAME} ${libs})
