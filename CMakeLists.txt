cmake_minimum_required(VERSION 3.28)
project(learn_sdr LANGUAGES CXX)


find_package(PkgConfig REQUIRED)

pkg_check_modules(FFTW3 REQUIRED fftw3)


add_subdirectory(rtlsdrpp)


add_executable(rx_to_file
  rx_to_file.cpp
)
target_link_libraries(rx_to_file
  rtlsdrpp
)

add_executable(rx_async
  rx_async.cpp
  Waterfall.cpp
)
target_link_libraries(rx_async
  rtlsdrpp
  fftw3f
)