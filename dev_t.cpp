#include "dev_t.hpp"
#include "rtlsdr.hpp"


#include <rtl-sdr.h>
#include <stdexcept>
#include <array>


namespace rtlsdr {


dev_t::dev_t(int index)
{
  auto const error = rtlsdr_open(&handler_, index);
  if (error) {
    std::runtime_error{"unable to initialize a rtlsdr device"};
  }
}


dev_t::~dev_t()
{
  rtlsdr_close(handler_);
}


device_info_t dev_t::get_usb_strings() noexcept
{
  std::array<char,256> manufact;
  std::array<char,256> product;
  std::array<char,256> serial;

  rtlsdr_get_usb_strings(
    handler_, 
    manufact.data(), 
    product.data(), 
    serial.data()
  );

  return device_info_t{
    .manufact = std::string(manufact.data()),
    .product = std::string(product.data()),
    .serial = std::string(serial.data())
  };
}


void dev_t::set_center_frequency(uint32_t const freq)
{
  auto const error = rtlsdr_set_center_freq(handler_, freq);
  if (error) {
    throw std::runtime_error{"error setting the center frequency"};
  }
}


uint32_t dev_t::get_center_frequency() const
{
  auto const freq = rtlsdr_get_center_freq(handler_);
  if (!freq) {
    throw std::runtime_error{"error getting the center frequency"};
  }

  return freq;
}


};