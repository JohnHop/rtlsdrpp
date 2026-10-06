cmake_minimum_required(VERSION 3.28)
project(rtlsdrpp LANGUAGES CXX)


option(RTLSDRPP_ENABLE_TESTING "Build tests" ${PROJECT_IS_TOP_LEVEL})


# enable ASAN if `PROJECT_IS_TOP_LEVEL` and `CMAKE_BUILD_TYPE` is Debug
if(PROJECT_IS_TOP_LEVEL AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()


# main library
add_subdirectory(rtlsdrpp)

if(RTLSDRPP_ENABLE_TESTING)
  add_subdirectory(test)
endif()