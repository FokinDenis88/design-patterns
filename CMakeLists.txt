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


include_directories(
	include
	external/cpp-general/include
	${GTEST_INCLUDE_DIR}
)

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
	include/cpp-idiom/attorney-client.hpp
	include/cpp-idiom/copy-n-swap.hpp
	include/cpp-idiom/crtp.hpp
	include/cpp-idiom/d-pointer.hpp
	include/cpp-idiom/named-parameter.hpp
	include/cpp-idiom/non-virtual-interface.hpp
	include/cpp-idiom/opaque-pointer.hpp
	include/cpp-idiom/pimpl.hpp
	include/cpp-idiom/pimpl-macros.hpp)
set(SOURCES_FILTER_CPP-IDIOMS
	src/cpp-idiom/d-pointer.cpp
	src/cpp-idiom/opaque-pointer.cpp
	src/cpp-idiom/pimpl.cpp)

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
    #doc/general/to-do.txt
	#doc/general/commit.txt
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
