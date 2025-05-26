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
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        origin/main		# Branch name, tag name or tag hash
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
set(GTEST_INCLUDE_DIR ${gtest_SOURCE_DIR}/include)
#=================================================================================


# Project Source Files Structure:
# AI
# Anti-Pattern
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

set(HEADERS_FILTER_AI)
set(SOURCES_FILTER_AI)

set(HEADERS_FILTER_ANTI_PATTERN)
set(SOURCES_FILTER_ANTI_PATTERN)

set(HEADERS_FILTER_ARCHITECTURAL
	include/architectural/mvc.hpp)
set(SOURCES_FILTER_ARCHITECTURAL)

set(INCLUDE_BEHAVIORAL include/behavioral/)
set(HEADERS_FILTER_BEHAVIORAL
	${INCLUDE_BEHAVIORAL}chain-of-responsibility.hpp
	${INCLUDE_BEHAVIORAL}command.hpp
	${INCLUDE_BEHAVIORAL}interpreter.hpp
	${INCLUDE_BEHAVIORAL}iterator.hpp
	${INCLUDE_BEHAVIORAL}mediator.hpp
	${INCLUDE_BEHAVIORAL}memento.hpp
	${INCLUDE_BEHAVIORAL}null-object.hpp
	${INCLUDE_BEHAVIORAL}observer.hpp
	${INCLUDE_BEHAVIORAL}state.hpp
	${INCLUDE_BEHAVIORAL}strategy.hpp
	${INCLUDE_BEHAVIORAL}template-method.hpp
	${INCLUDE_BEHAVIORAL}visitor.hpp)
set(SOURCES_FILTER_BEHAVIORAL
	src/behavioral/observer.cpp)

set(HEADERS_FILTER_CONCURRENCY)
set(SOURCES_FILTER_CONCURRENCY)

set(INCLUDE_CPP_IDIOM include/cpp-idiom/)
set(HEADERS_FILTER_CPP-IDIOMS
	${INCLUDE_CPP_IDIOM}attorney-client.hpp
	${INCLUDE_CPP_IDIOM}copy-n-swap.hpp
	${INCLUDE_CPP_IDIOM}crtp.hpp
	${INCLUDE_CPP_IDIOM}d-pointer.hpp
	${INCLUDE_CPP_IDIOM}erase-remove.hpp
	${INCLUDE_CPP_IDIOM}named-parameter.hpp
	${INCLUDE_CPP_IDIOM}named-parameter-type.hpp
	${INCLUDE_CPP_IDIOM}non-virtual-interface.hpp
	${INCLUDE_CPP_IDIOM}opaque-pointer.hpp
	${INCLUDE_CPP_IDIOM}pimpl.hpp
	${INCLUDE_CPP_IDIOM}pimpl-macros.hpp)
set(SOURCES_FILTER_CPP-IDIOMS
	src/cpp-idiom/d-pointer.cpp
	src/cpp-idiom/opaque-pointer.cpp
	src/cpp-idiom/pimpl.cpp)

set(INCLUDE_CREATIONAL include/creational/)
set(HEADERS_FILTER_CREATIONAL
	${INCLUDE_CREATIONAL}abstract-factory.hpp
	${INCLUDE_CREATIONAL}builder.hpp
	${INCLUDE_CREATIONAL}dependency-injection.hpp
	${INCLUDE_CREATIONAL}factory-method.hpp
	${INCLUDE_CREATIONAL}lazy-initialization.hpp
	${INCLUDE_CREATIONAL}object-pool.hpp
	${INCLUDE_CREATIONAL}prototype.hpp
	${INCLUDE_CREATIONAL}singleton.hpp)
set(SOURCES_FILTER_CREATIONAL)

set(HEADERS_FILTER_DATABASE)
set(SOURCES_FILTER_DATABASE)

set(HEADERS_FILTER_DISTRIBUTED-SYSTEM)
set(SOURCES_FILTER_DISTRIBUTED-SYSTEM)

set(INCLUDE_GAMEDEV include/gamedev/)
set(HEADERS_FILTER_GAMEDEV
	${INCLUDE_GAMEDEV}bytecode.hpp
	${INCLUDE_GAMEDEV}component.hpp
	${INCLUDE_GAMEDEV}data-locality.hpp
	${INCLUDE_GAMEDEV}dirty-flag.hpp
	${INCLUDE_GAMEDEV}double-buffer.hpp
	${INCLUDE_GAMEDEV}event-queue.hpp
	${INCLUDE_GAMEDEV}game-loop.hpp
	${INCLUDE_GAMEDEV}service-locator.hpp
	${INCLUDE_GAMEDEV}spatial-partition.hpp
	${INCLUDE_GAMEDEV}subclass-sandbox.hpp
	${INCLUDE_GAMEDEV}type-object.hpp
	${INCLUDE_GAMEDEV}update-method.hpp)
set(SOURCES_FILTER_GAMEDEV)

set(INCLUDE_STRUCTURAL include/structural/)
set(HEADERS_FILTER_STRUCTURAL
	${INCLUDE_STRUCTURAL}adapter.hpp
	${INCLUDE_STRUCTURAL}bridge.hpp
	${INCLUDE_STRUCTURAL}composite.hpp
	${INCLUDE_STRUCTURAL}decorator.hpp
	${INCLUDE_STRUCTURAL}delegation.hpp
	${INCLUDE_STRUCTURAL}facade.hpp
	${INCLUDE_STRUCTURAL}flyweight.hpp
	${INCLUDE_STRUCTURAL}marker.hpp
	${INCLUDE_STRUCTURAL}proxy.hpp)
set(SOURCES_FILTER_STRUCTURAL)

set(HEADERS_FILTER_UI)
set(SOURCES_FILTER_UI)

set(INCLUDE_GENERAL include/general/)
set(HEADERS_FILTER_GENERAL
	${INCLUDE_GENERAL}general.hpp
	${INCLUDE_GENERAL}ieditor.hpp)
set(SOURCES_FILTER_GENERAL)


set(INCLUDE_REFACTORING include/refactoring/)
set(HEADERS_FILTER_REFACTORING
	${INCLUDE_REFACTORING}extract-method.hpp
	${INCLUDE_REFACTORING}encapsulate-variable.hpp
	)
set(SOURCES_FILTER_REFACTORING)


# Many headers in every file
set(INCLUDE_HEADER_COLLECTION include/header-collection/)
set(HEADERS_FILTER_HEADER_COLLECTION
	${INCLUDE_HEADER_COLLECTION}ai-headers.hpp
	${INCLUDE_HEADER_COLLECTION}all-headers.hpp
	${INCLUDE_HEADER_COLLECTION}anti-pattern-headers.hpp
	${INCLUDE_HEADER_COLLECTION}architectural-headers.hpp
	${INCLUDE_HEADER_COLLECTION}classic-headers.hpp
	${INCLUDE_HEADER_COLLECTION}concurrency-headers.hpp
	${INCLUDE_HEADER_COLLECTION}cpp-idioms-headers.hpp
	${INCLUDE_HEADER_COLLECTION}database-headers.hpp
	${INCLUDE_HEADER_COLLECTION}distributed-system-headers.hpp
	${INCLUDE_HEADER_COLLECTION}gamedev-headers.hpp
	${INCLUDE_HEADER_COLLECTION}refactoring-headers.hpp
	${INCLUDE_HEADER_COLLECTION}ui-headers.hpp
)

set(ALL_SOURCES
	design-patterns.cpp
	${SOURCES_FILTER_AI}
	${SOURCES_FILTER_ANTI_PATTERN}
	${SOURCES_FILTER_ARCHITECTURAL}
	${SOURCES_FILTER_BEHAVIORAL}
	${SOURCES_FILTER_CONCURRENCY}
	${SOURCES_FILTER_CPP-IDIOMS}
	${SOURCES_FILTER_CREATIONAL}
	${SOURCES_FILTER_DATABASE}
	${SOURCES_FILTER_DISTRIBUTED-SYSTEM}
	${SOURCES_FILTER_GAMEDEV}
	${SOURCES_FILTER_STRUCTURAL}
	${SOURCES_FILTER_REFACTORING}
	${SOURCES_FILTER_UI}
	${SOURCES_FILTER_GENERAL})

set(ALL_HEADERS_FILTER
	${HEADERS_FILTER_AI}
	${HEADERS_FILTER_ANTI_PATTERN}
	${HEADERS_FILTER_ARCHITECTURAL}
    ${HEADERS_FILTER_BEHAVIORAL}
	${HEADERS_FILTER_CONCURRENCY}
	${HEADERS_FILTER_CPP-IDIOMS}
	${HEADERS_FILTER_CREATIONAL}
	${HEADERS_FILTER_DATABASE}
	${HEADERS_FILTER_DISTRIBUTED-SYSTEM}
	${HEADERS_FILTER_GAMEDEV}
	${HEADERS_FILTER_HEADER_COLLECTION}
	${HEADERS_FILTER_STRUCTURAL}
	${HEADERS_FILTER_REFACTORING}
	${HEADERS_FILTER_UI}
	${HEADERS_FILTER_GENERAL})

set(DOC_FILTER
	.gitlab-ci.yml
	README.md
    doc/issues/to-do.txt
	doc/issues/issue.txt
	doc/general/complexity-table.txt
	#doc/general/commit.txt
)


#find_package(OpenMP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME

# Include Directories
set(INCLUDES
	include
	external/cpp-general/include
	${GTEST_INCLUDE_DIR}	# for "gtest/gtest.h"
)
#target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDES})
include_directories(PUBLIC ${INCLUDES})


# Add source to this project's executable.
add_executable(${TARGET_NAME} ${ALL_SOURCES} ${ALL_HEADERS_FILTER} ${DOC_FILTER})

#target_link_libraries(${TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
#add_custom_command()


# Filters for Grouping files in project
# Headers
source_group("AI/Headers"					FILES ${HEADERS_FILTER_AI})
source_group("Anti-Pattern/Headers"			FILES ${HEADERS_FILTER_ANTI_PATTERN})
source_group("Architectural/Headers"		FILES ${HEADERS_FILTER_ARCHITECTURAL})
source_group("Behavioral/Headers"			FILES ${HEADERS_FILTER_BEHAVIORAL})
source_group("Concurrency/Headers"			FILES ${HEADERS_FILTER_CONCURRENCY})
source_group("Cpp-Idioms/Headers"			FILES ${HEADERS_FILTER_CPP-IDIOMS})
source_group("Creational/Headers"			FILES ${HEADERS_FILTER_CREATIONAL})
source_group("Database/Headers"				FILES ${HEADERS_FILTER_DATABASE})
source_group("Distributed-System/Headers"	FILES ${HEADERS_FILTER_DISTRIBUTED-SYSTEM})
source_group("GameDev/Headers"				FILES ${HEADERS_FILTER_GAMEDEV})
source_group("Header Collection"			FILES ${HEADERS_FILTER_HEADER_COLLECTION})
source_group("Structural/Headers"			FILES ${HEADERS_FILTER_STRUCTURAL})
source_group("Refactoring/Headers"			FILES ${HEADERS_FILTER_REFACTORING})
source_group("UI/Headers"					FILES ${HEADERS_FILTER_UI})
source_group("General/Headers"				FILES ${HEADERS_FILTER_GENERAL})

# Sources
source_group("AI/Source"					FILES ${SOURCES_FILTER_AI})
source_group("Anti-Pattern/Source"			FILES ${SOURCES_FILTER_ANTI_PATTERN})
source_group("Architectural/Source"			FILES ${SOURCES_FILTER_ARCHITECTURAL})
source_group("Behavioral/Source"			FILES ${SOURCES_FILTER_BEHAVIORAL})
source_group("Concurrency/Source"			FILES ${SOURCES_FILTER_CONCURRENCY})
source_group("Cpp-Idioms/Source"			FILES ${SOURCES_FILTER_CPP-IDIOMS})
source_group("Creational/Source"			FILES ${SOURCES_FILTER_CREATIONAL})
source_group("Database/Source"				FILES ${SOURCES_FILTER_DATABASE})
source_group("Distributed-System/Source"	FILES ${SOURCES_FILTER_DISTRIBUTED-SYSTEM})
source_group("GameDev/Source"				FILES ${SOURCES_FILTER_GAMEDEV})
source_group("Structural/Source"			FILES ${SOURCES_FILTER_STRUCTURAL})
source_group("Refactoring/Source"			FILES ${SOURCES_FILTER_REFACTORING})
source_group("UI/Source"					FILES ${SOURCES_FILTER_UI})
source_group("General/Source"				FILES ${SOURCES_FILTER_GENERAL})

# Other groups
source_group("Doc" FILES ${DOC_FILTER})


SET(ECLIPSE_GCC_STATIC_ANALYSIS -g -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Weffc++ -Wfloat-equal -c -fmessage-length=0)
set(ALL_STATIC_ANALYSIS_OPTIONS -std=c++2a -Wall -Wextra -pedantic -fanalyzer)
if(LINUX)
	#-std=c++2a -I/media/sf_Projects/IT/Programming/!git-web/private/design-patterns/include -O0 -g -pedantic -Wall -Wextra -Wconversion -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Weffc++ -Wfloat-equal -c -fmessage-length=0
	target_compile_options(${TARGET_NAME} PUBLIC ${ALL_STATIC_ANALYSIS_OPTIONS} ${ECLIPSE_GCC_STATIC_ANALYSIS})
elseif(MSVC)

endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 23)
  set_property(TARGET ${TARGET_NAME} PROPERTY C_STANDARD 17)
endif()
set_property(TARGET ${TARGET_NAME} PROPERTY SUBSYSTEM CONSOLE)


#============================Google Test========================================
# Source code, that will be tested
set(TARGET_TESTABLE_CODE
	src/cpp-idiom/pimpl.cpp)
# Source code of tests
set(SOURCES_FILTER_TESTS
	test/general/general-test.cpp)

# Library with code of work project for linking to test project
add_library(testable_code_library OBJECT ${TARGET_TESTABLE_CODE})
source_group("Tests" FILES ${SOURCES_FILTER_TESTS})

# Enable Google Tests
set(TEST_TARGET_NAME ${TARGET_NAME}-test)
enable_testing()

add_executable(
  ${TEST_TARGET_NAME}
  ${SOURCES_FILTER_TESTS})

target_link_libraries(
  ${TEST_TARGET_NAME}
  PUBLIC testable_code_library
  GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET_NAME})
#================================================================================
