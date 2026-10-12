cmake_minimum_required(VERSION 3.28)
project(rtlsdrpp LANGUAGES CXX)


# option(RTLSDRPP_BUILD_TESTS "Set to ON to build tests" OFF)
option(RTLSDRPP_BUILD_EXAMPLES "Set to ON to build examples" OFF)


# main library
add_subdirectory(rtlsdrpp)

if(RTLSDRPP_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(RTLSDRPP_BUILD_TESTS)
  add_subdirectory(tests)
endif()