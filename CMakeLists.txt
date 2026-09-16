cmake_minimum_required(VERSION 3.28)
project(learn_sdr LANGUAGES CXX)


find_package(rtlsdr REQUIRED)


add_library(rtlsdr_cppwrapper
  rtlsdr.cpp
  dev_t.cpp
)
target_link_libraries(rtlsdr_cppwrapper
  PRIVATE rtlsdr::rtlsdr
)


add_executable(main
  main.cpp
)
target_link_libraries(main
  PRIVATE rtlsdr_cppwrapper
)
target_compile_features(main
  PRIVATE cxx_std_23
)