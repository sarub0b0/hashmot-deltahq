cmake_minimum_required(VERSION 3.5)
if(UNIX)
set(CMAKE_C_COMPILER "/usr/bin/clang-6.0" CACHE string "clang compiler" FORCE)
endif(UNIX)
if(APPLE)
set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang" CACHE string "clang compiler" FORCE)
endif(APPLE)

set(PROJECT_NAME deltaHQ)

project(${PROJECT_NAME} C)


set(CMAKE_C_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING -pipe -march=native")
# set(CMAKE_C_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING -DMESSAGE_DEBUG")
# set(CMAKE_C_FLAGS_DEBUG "-g3 -O0")
# set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -DDEBUG_PRINT -fsanitize=address")
# set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -pg -fno-omit-frame-pointer -fsanitize=address,leak -fPIE")
# set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -fsanitize=address,leak -fPIE")
set(CMAKE_C_FLAGS_RELEASE "-O3")
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DMEASURE")
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DMEASURE2")
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DDEBUG_PRINT")
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DTCHK_ELAPSED")
# set(CMAKE_C_FLAGS_RELEASE "-O0 -g3 -fno-omit-frame-pointer -DMESSAGE_WARNING -fsanitize=address")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_C_FLAGS_MINSIZEREL "-Os")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)

include_directories(include)

set(libs m c pthread expat jansson)

set(sources_dir source)
set(headers_dir include)
file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} ${sources_dir}/*.c)
file(GLOB headers RELATIVE ${PROJECT_SOURCE_DIR} ${headers_dir}/*.h)


add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} ${libs})


# cmake_minimum_required(VERSION 3.5)

# if(UNIX)
# set(CMAKE_CXX_COMPILER "/usr/bin/clang++-6.0" CACHE string "clang++ compiler" FORCE)
# endif(UNIX)
# if(APPLE)
# set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++" CACHE string "clang++ compiler" FORCE)
# endif(APPLE)

# set(PROJECT_NAME deltaHQ)

# project(${PROJECT_NAME} CXX)

# set(CMAKE_CXX_COMPILER_NAME clang++)


# # set(CMAKE_CXX_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING -DMULTILINE_JSON")
# set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -Wall -D_GNU_SOURCE -march=native -pipe")
# # set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
# set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -fsanitize=address")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# # set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DMEASURE")
# # set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DMEASURE -DSSE")
# # set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DTCHK_ELAPSED")
# # set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DQUERY_VALIDATION")
# # set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DQUERY_VALIDATION -DTCHK_ELAPSED")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
# set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)

# include_directories(include)

# # find_package(PkgConfig)
# # pkg_check_modules(lib_external REQUIRED
# #     rapidjson
# #     )


# set(libs m c pthread expat jansson)
# set(sources source)
# set(includes include)
# file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} ${sources}/*.cc)
# file(GLOB headers RELATIVE ${PROJECT_SOURCE_DIR} ${includes}/*.h)


# add_executable(${PROJECT_NAME} ${sources})
# target_include_directories(${PROJECT_NAME} PRIVATE include)
# target_link_libraries(${PROJECT_NAME} ${libs})

# # message(STATUS "*** dump start cmake variables ***")
# # get_cmake_property(_variableNames VARIABLES)
# # foreach(_variableName ${_variableNames})
# #         message(STATUS "${_variableName}=${${_variableName}}")
# # endforeach()
# # message(STATUS "*** dump end ***")
