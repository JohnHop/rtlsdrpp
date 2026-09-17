cmake_minimum_required(VERSION 3.28)
project(learn_sdr LANGUAGES CXX)


add_subdirectory(rtlsdrpp)


add_executable(main
  main.cpp
)
target_link_libraries(main
  rtlsdrpp
)