cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME location_info_base)

project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_COMPILER_NAME clang++)

if(UNIX)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE string "clang++ compiler" FORCE)
endif(UNIX)
if(APPLE)
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++" CACHE string "clang++ compiler" FORCE)
endif(APPLE)

set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_CXX_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING -DMULTILINE_JSON")
set(CMAKE_CXX_FLAGS "-Wall -D_GNU_SOURCE -DMESSAGE_WARNING")
# set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -fno-omit-frame-pointer -fsanitize=leak -fPIE")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DMESSAGE_WARNING")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)

include_directories(include)

# find_package(PkgConfig)
# pkg_check_modules(lib_external REQUIRED
#     rapidjson
#     )

set(libs m)

set(sources source)
set(includes include)
file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} ${sources}/*.cc ${sources}/*/*.cc)
file(GLOB headers RELATIVE ${PROJECT_SOURCE_DIR} ${includes}/*.hh)


add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} ${libs})

# message(STATUS "*** dump start cmake variables ***")
# get_cmake_property(_variableNames VARIABLES)
# foreach(_variableName ${_variableNames})
#         message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
# message(STATUS "*** dump end ***")
