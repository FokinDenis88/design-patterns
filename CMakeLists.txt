# CMakeList.txt : CMake project for artificial-neural-networks, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

# Can be different with target
set(PROJECT_NAME design-patterns)
project(${PROJECT_NAME} VERSION 1.0)
set(TARGET_NAME ${PROJECT_NAME})


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


set(INCLUDES include)

# Project Source Files Structure:
# Architectural
# Behavioral
# Concurrency
# Cpp-Idioms
# Creational
# Database
# Distributed-System
# GameDev
# Include	-	all includes
# Structural
# UI

set(HEADERS_FILTER_ARCHITECTURAL
	include/architectural/mvc.hpp)
set(SOURCES_FILTER_ARCHITECTURAL)

set(HEADERS_FILTER_BEHAVIORAL
	include/behavioral/chain-of-responsibility.hpp
	include/behavioral/command.hpp
	include/behavioral/interpreter.hpp
	include/behavioral/iterator.hpp
	include/behavioral/mediator.hpp
	include/behavioral/memento.hpp
	include/behavioral/null-object.hpp
	include/behavioral/observer.hpp
	include/behavioral/state.hpp
	include/behavioral/strategy.hpp
	include/behavioral/template-method.hpp
	include/behavioral/visitor.hpp)
set(SOURCES_FILTER_BEHAVIORAL
	src/behavioral/observer.cpp)

set(HEADERS_FILTER_CONCURRENCY)
set(SOURCES_FILTER_CONCURRENCY)

set(HEADERS_FILTER_CPP-IDIOMS
	include/cpp-idiom/d-pointer.hpp
	include/cpp-idiom/opaque-pointer.hpp
	include/cpp-idiom/pimpl.hpp
	include/cpp-idiom/pimpl-example.hpp)
set(SOURCES_FILTER_CPP-IDIOMS
	src/cpp-idiom/d-pointer.cpp
	src/cpp-idiom/opaque-pointer.cpp
	src/cpp-idiom/pimpl-example.cpp)

set(HEADERS_FILTER_CREATIONAL
	include/creational/abstract-factory.hpp
	include/creational/builder.hpp
	include/creational/dependency-injection.hpp
	include/creational/factory-method.hpp
	include/creational/lazy-initialization.hpp
	include/creational/object-pool.hpp
	include/creational/prototype.hpp
	include/creational/singleton.hpp)
set(SOURCES_FILTER_CREATIONAL)

set(HEADERS_FILTER_DATABASE)
set(SOURCES_FILTER_DATABASE)

set(HEADERS_FILTER_DISTRIBUTED-SYSTEM)
set(SOURCES_FILTER_DISTRIBUTED-SYSTEM)

set(HEADERS_FILTER_GAMEDEV
	include/gamedev/bytecode.hpp
	include/gamedev/component.hpp
	include/gamedev/data-locality.hpp
	include/gamedev/dirty-flag.hpp
	include/gamedev/double-buffer.hpp
	include/gamedev/event-queue.hpp
	include/gamedev/game-loop.hpp
	include/gamedev/service-locator.hpp
	include/gamedev/spatial-partition.hpp
	include/gamedev/subclass-sandbox.hpp
	include/gamedev/type-object.hpp
	include/gamedev/update-method.hpp)
set(SOURCES_FILTER_GAMEDEV)

set(HEADERS_FILTER_INCLUDE
	include/headers/all-headers.hpp
	include/headers/architectural-headers.hpp
	include/headers/classic-headers.hpp
	include/headers/cpp-idioms-headers.hpp
	include/headers/gamedev-headers.hpp)

set(HEADERS_FILTER_STRUCTURAL
	include/structural/adapter.hpp
	include/structural/bridge.hpp
	include/structural/composite.hpp
	include/structural/decorator.hpp
	include/structural/delegation.hpp
	include/structural/facade.hpp
	include/structural/flyweight.hpp
	include/structural/marker.hpp
	include/structural/proxy.hpp)
set(SOURCES_FILTER_STRUCTURAL)

set(HEADERS_FILTER_UI)
set(SOURCES_FILTER_UI)


set(SOURCES
	design-patterns.cpp
	${SOURCES_FILTER_ARCHITECTURAL}
	${SOURCES_FILTER_BEHAVIORAL}
	${SOURCES_FILTER_CONCURRENCY}
	${SOURCES_FILTER_CPP-IDIOMS}
	${SOURCES_FILTER_CREATIONAL}
	${SOURCES_FILTER_DATABASE}
	${SOURCES_FILTER_DISTRIBUTED-SYSTEM}
	${SOURCES_FILTER_GAMEDEV}
	${SOURCES_FILTER_STRUCTURAL}
	${SOURCES_FILTER_UI})

set(ALL_HEADERS_FILTER
	${HEADERS_FILTER_ARCHITECTURAL}
    ${HEADERS_FILTER_BEHAVIORAL}
	${HEADERS_FILTER_CONCURRENCY}
	${HEADERS_FILTER_CPP-IDIOMS}
	${HEADERS_FILTER_CREATIONAL}
	${HEADERS_FILTER_DATABASE}
	${HEADERS_FILTER_DISTRIBUTED-SYSTEM}
	${HEADERS_FILTER_GAMEDEV}
	${HEADERS_FILTER_INCLUDE}
	${HEADERS_FILTER_STRUCTURAL}
	${HEADERS_FILTER_UI})
set(DOC_FILTER
    #doc/to-do.txt
	)


#find_package(OpenMP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME


# Add source to this project's executable.
add_executable(${TARGET_NAME} ${SOURCES} ${ALL_HEADERS_FILTER} ${DOC_FILTER})

# Include Directories
target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDES})
#target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)

#add_custom_command()


# Filters for Grouping files in project
# Headers
source_group("Architectural/Headers" FILES ${HEADERS_FILTER_ARCHITECTURAL})
source_group("Behavioral/Headers" FILES ${HEADERS_FILTER_BEHAVIORAL})
source_group("Concurrency/Headers" FILES ${HEADERS_FILTER_CONCURRENCY})
source_group("Cpp-Idioms/Headers" FILES ${HEADERS_FILTER_CPP-IDIOMS})
source_group("Creational/Headers" FILES ${HEADERS_FILTER_CREATIONAL})
source_group("Database/Headers" FILES ${HEADERS_FILTER_DATABASE})
source_group("Distributed-System/Headers" FILES ${HEADERS_FILTER_DISTRIBUTED-SYSTEM})
source_group("GameDev/Headers" FILES ${HEADERS_FILTER_GAMEDEV})
source_group("Include" FILES ${HEADERS_FILTER_INCLUDE})
source_group("Structural/Headers" FILES ${HEADERS_FILTER_STRUCTURAL})
source_group("UI/Headers" FILES ${HEADERS_FILTER_UI})

# Sources
source_group("Architectural/Source" FILES ${SOURCES_FILTER_ARCHITECTURAL})
source_group("Behavioral/Source" FILES ${SOURCES_FILTER_BEHAVIORAL})
source_group("Concurrency/Source" FILES ${SOURCES_FILTER_CONCURRENCY})
source_group("Cpp-Idioms/Source" FILES ${SOURCES_FILTER_CPP-IDIOMS})
source_group("Creational/Source" FILES ${SOURCES_FILTER_CREATIONAL})
source_group("Database/Source" FILES ${SOURCES_FILTER_DATABASE})
source_group("Distributed-System/Source" FILES ${SOURCES_FILTER_DISTRIBUTED-SYSTEM})
source_group("GameDev/Source" FILES ${SOURCES_FILTER_GAMEDEV})
source_group("Structural/Source" FILES ${SOURCES_FILTER_STRUCTURAL})
source_group("UI/Source" FILES ${SOURCES_FILTER_UI})


SET(ECLIPSE_GCC_STATIC_ANALYSIS -g -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Weffc++ -Wfloat-equal -c -fmessage-length=0)
set(ALL_STATIC_ANALYSIS_OPTIONS -std=c++2a -Wall -Wextra -pedantic -fanalyzer)
if(LINUX)
	#-std=c++2a -I/media/sf_Projects/IT/Programming/!git-web/private/design-patterns/include -O0 -g -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Weffc++ -Wfloat-equal -c -fmessage-length=0
	target_compile_options(${TARGET_NAME} PUBLIC ${ALL_STATIC_ANALYSIS_OPTIONS} ${ECLIPSE_GCC_STATIC_ANALYSIS})
elseif(MSVC)

endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
  set_property(TARGET ${TARGET_NAME} PROPERTY C_STANDARD 17)
endif()
set_property(TARGET ${TARGET_NAME} PROPERTY SUBSYSTEM CONSOLE)




#=========================Web Links=============================
# GCC Compiler
# https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Invoking-GCC.html
# https://www.rapidtables.com/code/linux/gcc.html

# Static Analysis Options:
# -std=c++2a -std=c++1z -Wall -Wextra -fanalyzer

# Eclipse gcc Static Analysis with all warnings
#-std=c++2a -I/media/sf_Projects/IT/Programming/!git-web/private/design-patterns/include -O0 -g -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Weffc++ -Wfloat-equal -c -fmessage-length=0



# Help Information
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html
# https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#cmake-language-variables


#set(srcs a.c b.c c.c) # sets "srcs" to "a.c;b.c;c.c"

#add_executable(main ${SOURCE_EXE})	# Создает исполняемый файл с именем main
#add_library (Hello STATIC hello.cxx)
#target_include_directories (Hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#add_executable (helloDemo demo.cxx demo_b.cxx)
#target_link_libraries (helloDemo LINK_PUBLIC Hello)
#target_compile_options(first-test PRIVATE -fexceptions)
#target_link_options()

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

# Добавляет директории для дальнейшей обработки. В них нужно добавлять еще одни CMakeLists.txt файлы
# add_subdirectory (Hello)



#message("generator is set to ${CMAKE_GENERATOR}")
#set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)

#Eclipse CDT4 - Ninja
#Eclipse CDT4 - Unix Makefiles


#if (WIN32)
    # do something
#endif (WIN32)

#if (UNIX)
    # do something
#endif (UNIX)

#if (MSVC)
    # do something
#endif (MSVC)

#if(UNIX AND NOT APPLE)
    #set(LINUX TRUE)
#endif()

# if(NOT LINUX) should work, too, if you need that
#if(LINUX)
    #message(STATUS ">>> Linux")
    # linux stuff here
#else()
    #message(STATUS ">>> Not Linux")
    # stuff that should happen not on Linux
#endif()

# Permanent verbose command output from the generated Makefiles
#cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON .









#=============================================================================
# https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Invoking-GCC.html
# https://www.rapidtables.com/code/linux/gcc.html
# -c				: compile source files to object files without linking
# -E				: Preprocess only; do not compile, assemble or link
# -glevel			: generate debug information to be used by GDB
# -Idir 			: include directory of header files
# -llib				: link with library file
# -Ldir 			: look in directory for library files
# -o output file 	: write build output to output file
# -Olevel			: optimize for code size and execution tim
# -time 			: Time the execution of each subprocess
# --version			: Display compiler version information
# https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Warning-Options.html#Warning-Options
# -Wall				:
# -Wextra			:
# -fanalyzer		:
# -std=c++2a		: