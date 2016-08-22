cmake_minimum_required(VERSION 2.8)
project(Editor) # TODO: Change the project name to yours.

option(JIL_ENABLE_TEST "enable unit test?" OFF)

# Enable unicode.
add_definitions(-DUNICODE -D_UNICODE)
include_directories(${PROJECT_SOURCE_DIR})

# Find wxWidgets.
find_package(wxWidgets REQUIRED core base adv)
include(${wxWidgets_USE_FILE})

# Find boost.
# The latest CMake FindBoost script doesn't support boost 1.49.0.
#set(Boost_ADDITIONAL_VERSIONS "1.49", "1.49.0")
#find_package(Boost 1.47)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#    link_directories(${Boost_LIBRARY_DIRS})
#endif()

# Find gtest.
if (JIL_ENABLE_TEST)
    enable_testing()
    if (MSVC)
      set(GTEST_MSVC_SEARCH MD)
    endif ()
#   find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
endif()

# Include the source directory.
include_directories(${PROJECT_SOURCE_DIR}/src)

# Disable CRT secure warnings.
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Enable TinyXml STL
add_definitions(-DTIXML_USE_STL)

add_subdirectory(src)
