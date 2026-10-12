/**
 * Show rtl-sdr devices attached and print some info, a bit like `rtl_test`.
 * Inspired by `uhd_find_devices.cpp` from UHD library by Ettus.
 */


#include "rtlsdrpp/dev_t.hpp"
#include <algorithm>
#include <cstdlib>
#include <print>

#include <rtlsdrpp/rtlsdrpp.hpp>

#include "common.h"


std::vector<double> to_double(std::vector<int> const& range)
{
  std::vector<double> result(range.size());

  std::ranges::transform(
    range,
    result.begin(),
    [](auto const g) { return g / 10.0; }
  );

  return result;
}


int main()
{
  auto const device_count = rtlsdrpp::get_device_count();
  std::println("Found {} device(s):", device_count);

  // brief list
  for (int i = 0; i < device_count; ++i)
  {
    auto const usb_strings = rtlsdrpp::get_device_usb_strings(i);
    auto const device_name = rtlsdrpp::get_device_name(i);

    std::println("  {}:  manufact: {}, product: {}, serial: {}, name: {}", i,
      usb_strings.manufact,
      usb_strings.product,
      usb_strings.serial,
      device_name
    );
  }
  std::println();

  // details
  for (int i = 0; i < device_count; ++i)
  {
    std::println("> Details for device {}: {}", i, rtlsdrpp::get_device_name(i));

    rtlsdrpp::dev_t device{i};
    auto const gains = to_double(device.get_turner_gains());
    std::print("Suported gain values ({}): ", gains.size());
    std::println("{}", to_string(gains));

    std::println();
  }

  return EXIT_SUCCESS;
}