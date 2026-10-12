find_package(PkgConfig REQUIRED)

find_package(Boost COMPONENTS program_options REQUIRED)


add_executable(rx_samples_to_file
  rx_samples_to_file.cpp
)
target_link_libraries(rx_samples_to_file
  PRIVATE rtlsdrpp
          ${Boost_LIBRARIES}
)


add_executable(rtlsdr_find_devices
  rtlsdr_find_devices.cpp
)
target_link_libraries(rtlsdr_find_devices
  PRIVATE rtlsdrpp
)