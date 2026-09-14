#include "rtlsdr.hpp"


#include <array>

#include <rtl-sdr.h>
#include <stdexcept>

#include "libusb_category.hpp"


int rtlsdr::get_device_count() noexcept
{
  return rtlsdr_get_device_count();
}


std::string_view rtlsdr::get_device_name(int const index)
{
  std::string_view name = rtlsdr_get_device_name(index);

  if (name.empty()) {
    throw std::system_error{ENODEV, std::system_category()};
  }

  return name;
}


/**
 * @details
 * Dall'analisi del codice sorgente di `get_device_usb_strings()`, la funzione
 * restituisce un codice di errore della libreria `libusb`.
 */
rtlsdr::device_info_t rtlsdr::get_device_usb_strings(int const index)
{
  std::array<char,256> manufact;
  std::array<char,256> product;
  std::array<char,256> serial;

  auto const error = 
    rtlsdr_get_device_usb_strings(
      index, 
      manufact.data(), 
      product.data(), 
      serial.data()
    );

  if (error) {
    throw std::system_error{error, rtlsdr::libusb_category()};
  }

  return device_info_t{
    .manufact = std::string(manufact.data()),
    .product = std::string(product.data()),
    .serial = std::string(serial.data()),
  };
}


int rtlsdr::get_index_by_serial(std::string const& serial)
{
  if (serial.empty()) {
    throw std::invalid_argument{"serial is empty"};
  }

  auto const error = rtlsdr_get_index_by_serial(serial.c_str());
  switch (error)
  {
    case -2: throw std::runtime_error{"no rtlsdr devices found"}; break;
    case -3: throw std::runtime_error{
      "rtlsdr device found but not with " + serial + " string descriptor"
    }; break;
  }

  return error;
}