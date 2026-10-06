find_package(PkgConfig REQUIRED)

# pkg_check_modules(FFTW3 REQUIRED fftw3)
find_package(Boost COMPONENTS program_options REQUIRED)


add_executable(rx_samples_to_file
  rx_samples_to_file.cpp
)
target_link_libraries(rx_samples_to_file
  PRIVATE rtlsdrpp
          ${Boost_LIBRARIES}
)