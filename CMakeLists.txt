cmake_minimum_required(VERSION 3.28)
project(learn_sdr LANGUAGES CXX)


add_subdirectory(rtlsdrpp)


add_executable(rx_to_file
  rx_to_file.cpp
)
target_link_libraries(rx_to_file
  rtlsdrpp
)