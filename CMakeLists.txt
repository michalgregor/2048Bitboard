cmake_minimum_required (VERSION 2.6)

# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Release prior to calling PROJECT()
#
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()

project(Game2048 CXX)

# Add custom search modules.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

#####################################################################
#           Options
#####################################################################


#####################################################################
#           Libraries
#####################################################################

# ncurses
FIND_PACKAGE(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
SET(LIBS ${LIBS} ${CURSES_LIBRARIES})

#####################################################################
#           C++11 support
#####################################################################

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#####################################################################
#           Compiler-specific flags
#####################################################################

# Warnings for gcc.
if(CMAKE_COMPILER_IS_GNUCXX)
	SET(WARNINGS "${WARNINGS} -Wextra -Wall -pedantic -Wmain -Weffc++ -Wswitch-default -Wswitch-enum -Wmissing-include-dirs -Wmissing-declarations -Wfloat-equal -Wundef -Wcast-align -Wredundant-decls -Winit-self -Wshadow")
#	SET(WARNINGS "${WARNINGS} -Wconversion")
endif(CMAKE_COMPILER_IS_GNUCXX)

#####################################################################
#           			Source files
#####################################################################

FILE( GLOB DTREE_SRCS
	*.cpp
)

FILE( GLOB DTREE_HEADERS
	*.h
)

#####################################################################
#           The main executable
#####################################################################

add_executable(Game2048 ${DTREE_SRCS})
set_target_properties(Game2048 PROPERTIES COMPILE_FLAGS ${WARNINGS})
TARGET_LINK_LIBRARIES(Game2048 ${LIBS})

######################################################################
##           	Installation
######################################################################

#install(TARGETS DtreeFuzzy FDTMain
#		RUNTIME DESTINATION bin
#		LIBRARY DESTINATION lib
#		ARCHIVE DESTINATION bin
#		)

#install(FILES ${DTREE_HEADERS} DESTINATION include/dtree_fuzzy COMPONENT dev)

