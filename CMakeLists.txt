# CMakeList.txt : CMake project for artificial-neural-networks, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Can be different with target
project("design-patterns" VERSION 1.0)


#============================Google Test========================================
# http://google.github.io/googletest/quickstart-cmake.html
# GoogleTest requires at least C++14
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include(FetchContent)
#FetchContent_Declare(
  #googletest
  #URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
#)
# For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)
#=================================================================================


# Project Source Files Structure
#architectural
#behavioral
#concurrency
#creational
#structural

set(SOURCES
	design-patterns.cpp
	src/architectural/mvc.cpp
    src/behavioral/command.cpp
	src/behavioral/observer.cpp
	src/behavioral/state.cpp
	src/behavioral/strategy.cpp
	src/creational/abstract-factory.cpp
	src/creational/builder.cpp
	src/creational/factory-method.cpp
	src/creational/lazy-initialization.cpp
	src/creational/object-pool.cpp
	src/creational/prototype.cpp
	src/creational/singleton.cpp
	src/structural/adapter.cpp
	src/structural/decorator.cpp
	src/structural/facade.cpp
	src/structural/flyweight.cpp
	src/structural/proxy.cpp
    )

set(INCLUDES
    include
    )

set(HEADER_FILES_FILTER
    include/architectural/mvc.hpp
    include/behavioral/command.hpp
	include/behavioral/observer.hpp
	include/behavioral/state.hpp
	include/behavioral/strategy.hpp
	include/creational/abstract-factory.hpp
	include/creational/builder.hpp
	include/creational/factory-method.hpp
	include/creational/lazy-initialization.hpp
	include/creational/object-pool.hpp
	include/creational/prototype.hpp
	include/creational/singleton.hpp
	include/structural/adapter.hpp
	include/structural/decorator.hpp
	include/structural/facade.hpp
	include/structural/flyweight.hpp
	include/structural/proxy.hpp
    )
set(DOC_FILTER
    #doc/to-do.txt
    )


#find_package(OpenMP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Add source to this project's executable.
add_executable(design-patterns ${SOURCES} ${HEADER_FILES_FILTER} ${DOC_FILTER})

# Include Directories
target_include_directories(design-patterns PUBLIC ${INCLUDES})
#target_link_libraries(artificial-neural-networks PRIVATE OpenMP::OpenMP_CXX)

# Filters for Grouping files in project
source_group("Header Files" FILES ${HEADER_FILES_FILTER})
source_group("Doc" FILES ${DOC_FILTER})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET design-patterns PROPERTY CXX_STANDARD 20)
  set_property(TARGET design-patterns PROPERTY C_STANDARD 17)
endif()
set_property(TARGET design-patterns PROPERTY SUBSYSTEM CONSOLE)





# Help Information
# TODO: Add tests and install targets if needed.


# Добавляет директории для дальнейшей обработки. В них нужно добавлять еще одни CMakeLists.txt файлы
# add_subdirectory (Hello)

#add_library (Hello hello.cxx)
#target_include_directories (Hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#add_executable (helloDemo demo.cxx demo_b.cxx)
#target_link_libraries (helloDemo LINK_PUBLIC Hello)

#set(SOURCE_EXE main.cpp)   set(SOURCE_LIB foo.cpp)
#add_library(foo STATIC ${SOURCE_LIB})	# Создание статической библиотеки с именем foo
#add_executable(main ${SOURCE_EXE})	# Создает исполняемый файл с именем main
#target_link_libraries(main foo)
#include_directories(foo)			# Расположение заголовочных файлов
#file(GLOB <variable> [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS] [<globbing-expressions>...])
#target_compile_options(first-test PRIVATE -fexceptions)

#find_package(OpenMP)
#if (OPENMP_FOUND)
    #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    #set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

#set_property(TARGET artificial-neural-networks PROPERTY SUBSYSTEM WINDOWS)
#CMAKE_CXX_FLAGS    /SUBSYSTEM:CONSOLE

#source_group("Header Files" FILES ${INCLUDES} REGULAR_EXPRESSION *)

#OpenMP
#find_package(OpenMP REQUIRED)
#target_link_libraries(artificial-neural-networks PRIVATE OpenMP::OpenMP_CXX)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)