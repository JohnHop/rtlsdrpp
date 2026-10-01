#include "dev_t.hpp"


#include <stdexcept>
#include <array>
#include <system_error>
#include <utility>
#include <cerrno>

#include <rtl-sdr.h>

#include "libusb_category.hpp"
#include "rtlsdrpp.hpp"


namespace rtlsdrpp {


dev_t::dev_t(int const index)
{
  auto const error = rtlsdr_open(&handler_, index);
  if (error) {
    throw std::runtime_error{"unable to initialize a rtlsdrpp device"};
  }
}


dev_t::~dev_t()
{
  rtlsdr_close(handler_);
}


void dev_t::set_xtal_freq(xtal_freq_t const xtal_freq)
{
  auto const error =
    rtlsdr_set_xtal_freq(handler_, xtal_freq.rtl_freq, xtal_freq.tuner_freq);

  if (error) {
    throw std::runtime_error{"error setting the crystal oscillator"};
  }
}


xtal_freq_t dev_t::get_xtal_freq() noexcept
{
  xtal_freq_t result;

  rtlsdr_get_xtal_freq(handler_, &result.rtl_freq, &result.tuner_freq);

  return result;
}


dev_info_t dev_t::get_usb_strings() noexcept
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

  return dev_info_t{
    .manufact = std::string(manufact.data()),
    .product = std::string(product.data()),
    .serial = std::string(serial.data())
  };
}


void dev_t::write_eeprom(std::span<uint8_t> buffer, uint8_t offset)
{
  auto const error =
    rtlsdr_write_eeprom(handler_, buffer.data(), offset, buffer.size());

  switch (error)
  {
    case -2: throw std::runtime_error{"EEPROM size is exceeded"};
    case -3: throw std::runtime_error{"no EEPROM was found"};
  }
}


void dev_t::read_eeprom(std::span<uint8_t> buffer, uint8_t offset)
{
  auto const error =
    rtlsdr_read_eeprom(handler_, buffer.data(), offset, buffer.size());

  switch (error)
  {
    case -2: throw std::runtime_error{"EEPROM size is exceeded"};
    case -3: throw std::runtime_error{"no EEPROM was found"};
  }
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


void dev_t::set_freq_correction(int const ppm)
{
  auto const error = rtlsdr_set_freq_correction(handler_, ppm);

  if (error) {
    throw std::runtime_error{"error set frequency correction"};
  }
}


int dev_t::get_freq_correction() const noexcept
{
  return rtlsdr_get_freq_correction(handler_);
}


tuner_type_e dev_t::get_turner_type() const noexcept
{
  return static_cast<tuner_type_e>(rtlsdr_get_tuner_type(handler_));
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


void dev_t::set_turner_if_gain(int const stage, int const gain)
{
  auto const error = rtlsdr_set_tuner_if_gain(handler_, stage, gain);
  if (error) {
    throw std::runtime_error{"error to set the turner if gain"};
  }
}


void dev_t::set_turner_bandwidth(uint32_t const bw)
{
  auto const error = rtlsdr_set_tuner_bandwidth(handler_, bw);
  if (error) {
    throw std::runtime_error{"unable to set the turner bandwidth"};
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


void dev_t::set_testmode(bool const on)
{
  auto const error = rtlsdr_set_testmode(handler_, on);
  if (error) {
    throw std::runtime_error{"error to set testmode " + std::to_string(on)};
  }
}


void dev_t::reset_buffer() noexcept
{
  rtlsdr_reset_buffer(handler_);
}


void dev_t::read_sync(std::span<cu8_t> buffer, int& read_count)
{
  read_count = 0;

  if (buffer.size_bytes() % 512) {
    throw std::invalid_argument{"buffer size must be a multiple of 512"};
  }

  auto const error =
    rtlsdr_read_sync(handler_, buffer.data(), buffer.size_bytes(), &read_count);
  if (error) {
    throw std::system_error{-error, libusb_category()};
  }
}


void dev_t::read_async(read_async_no_ctx_cb_t* cb,
                       std::size_t buffer_len,
                       int buffer_num)
{
  auto const error =
    rtlsdr_read_async(
      handler_,
      &dev_t::async_bridge_no_ctx,
      reinterpret_cast<void*>(cb),
      buffer_num,
      buffer_len
    );
  if (error) {
    throw std::runtime_error{"error reading asynchronously"};
  }
}


void dev_t::cancel_async() noexcept
{
  rtlsdr_cancel_async(handler_);
}


};