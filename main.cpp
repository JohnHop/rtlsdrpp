#include <cstdlib>
#include <print>

#include <rtl-sdr.h>

#include <rtlsdrpp/rtlsdrpp.hpp>


int main(int argc, char* argv[])
{
  auto dev_0 = rtlsdr_get_device_name(0);
  auto dev_1 = rtlsdr_get_device_name(1);
  auto dev_2 = rtlsdr_get_device_name(2);
  
  return EXIT_SUCCESS;
}