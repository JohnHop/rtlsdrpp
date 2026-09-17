cmake_minimum_required(VERSION 3.28)
project(rtlsdrpp VERSION 0.0.1 LANGUAGES CXX)


find_package(rtlsdr REQUIRED)


add_library(rtlsdrpp STATIC
  source/rtlsdrpp.cpp
  source/dev_t.cpp
)
target_include_directories(rtlsdrpp
  PRIVATE include/rtlsdrpp
  PUBLIC  include
)
target_link_libraries(rtlsdrpp
  PUBLIC  rtlsdr::rtlsdr
)
target_compile_features(rtlsdrpp
  PUBLIC  cxx_std_23
)