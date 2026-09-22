#include "dev_t.hpp"


#include <stdexcept>
#include <array>
#include <system_error>
#include <utility>
#include <cerrno>

#include <rtl-sdr.h>

#include "libusb_category.hpp"


namespace rtlsdrpp {


dev_t::dev_t(int const index)
{
  auto const error = rtlsdr_open(&handler_, index);
  if (error) {
    std::runtime_error{"unable to initialize a rtlsdrpp device"};
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


std::vector<int> dev_t::get_turner_gains()
{
  auto const gains_size = rtlsdr_get_tuner_gains(handler_, nullptr);

  std::vector<int> gains(gains_size);
  rtlsdr_get_tuner_gains(handler_, gains.data());

  return gains;
}


void dev_t::set_tuner_gain(int const gain)
{
  auto const error = rtlsdr_set_tuner_gain(handler_, gain);
  if (error) {
    throw std::runtime_error{"unable to set the tuner gain"};
  }
}


void dev_t::set_turner_bandwidth(uint32_t const bw)
{
  auto const error = rtlsdr_set_tuner_bandwidth(handler_, bw);
  if (error) {
    std::runtime_error{"unable to set the turner bandwidth"};
  }
}


int dev_t::get_tuner_gain() const
{
  auto const gain = rtlsdr_get_tuner_gain(handler_);
  if (!gain) {
    throw std::runtime_error{"can't read the turner gain"};
  }

  return gain;
}


void dev_t::set_turner_gain_mode(turner_gain_mode_e gain_mode)
{
  auto const error = 
    rtlsdr_set_tuner_gain_mode(handler_, std::to_underlying(gain_mode));
  if (error) {
    throw std::runtime_error{"can't set the turner gain mode"};
  }
}


void dev_t::set_sample_rate(uint32_t rate)
{
  auto const error = rtlsdr_set_sample_rate(handler_, rate);
  if (error == -EINVAL) {
    throw std::invalid_argument{"invalid sample rate value"};
  } else if (error) {
    throw std::runtime_error{"unable to set the sample rate"};
  }
}


uint32_t dev_t::get_sample_rate() const
{
  auto const rate = rtlsdr_get_sample_rate(handler_);
  if (!rate) {
    throw std::runtime_error{"unable to get the sample rate"};
  }

  return rate;
}


void dev_t::reset_buffer() noexcept
{
  rtlsdr_reset_buffer(handler_);
}


void dev_t::read_sync(std::span<uint8_t> buffer, int& read_count)
{
  read_count = 0;

  if ((buffer.size()) % 512) {
    throw std::invalid_argument{"buffer size must be a multiple of 512"};
  }

  auto const error = 
    rtlsdr_read_sync(handler_, buffer.data(), buffer.size(), &read_count);
  if (error) {
    throw std::system_error{-error, libusb_category()};
  }
}


};