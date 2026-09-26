#include <cstdint>
#include <type_traits>
#if !defined(JOHNHOP_RTLSDR_DEVT_HPP)
#define JOHNHOP_RTLSDR_DEVT_HPP


#include <vector>
#include <span>
#include <cstddef>
#include <stdexcept>
#include <print>

#include <rtl-sdr.h>

#include "rtlsdrpp.hpp"


namespace rtlsdrpp {


/**
 * Wrapper around a `rtlsdr_dev_t` with RAII support.
 */
class dev_t
{
public:

//* Special member functions / / / / / / / / / / / / / / / / / / / / / / / / / /

  /// @brief Initialize a rtl-sdr device identified by `index`.
  /// @throws std::runtime_error if `index ` is not valid.
  dev_t(int index = 0);

  /// @brief De-initialize a rtl sdr device.
  ~dev_t();

//* Member functions / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

  device_info_t get_usb_strings() noexcept;

  /// @brief Set the center frequency.
  /// @throws std::runtime_error if can't set the frequency.
  void set_center_frequency(uint32_t freq);

  /// @brief Get the current center frequency.
  /// @throws std::runtime_error if can't get the frequency.
  uint32_t get_center_frequency() const;

  /// @brief Get a list of available gains for this rtl-sdr device.
  /// @note Result with a single gain value equals to 0 means no available gain.
  /// @returns Available turner gain values in tenths of a dB.
  std::vector<int> get_turner_gains();

  /// @brief Set the gain using a value in tenths of a dB.
  /// @throws std::runtime_error if can't set the gain.
  /// @note Manual gain mode must be enabled.
  /// @note Remember to use `get_turner_gains` to get all available gains.
  void set_tuner_gain(int gain);

  /// @brief Set the tuner bandwidth in hz.
  /// @throws std::runtime_error if can't set the bandwidth.
  void set_turner_bandwidth(uint32_t bw);

  /// @brief Get the tuner gain in tenths of a dB.
  /// @throws std::runtime_error if can't read the turner gain.
  /// @note If `set_tuner_gain` failed the also this function will fail.
  int get_tuner_gain() const;

  /// @brief Set the tuner gain mode (automatic/manual).
  /// @throws std::runtime_error if can't set the turner gain mode.
  /// @note This function succeedes also if it is not supported.
  void set_turner_gain_mode(turner_gain_mode_e gain_mode);

  /// @brief Set the sample rate in hz.
  /// @param[in] rate Values can be: (225khz, 300 khz) and (900khz, 3200khz].
  /// @throws std::invalid_argument if `rate` is invalid.
  /// @warning Expect sample loss for rates > 2'400'000.
  /// @note This function print on the `stderr` if the real rate is different.
  void set_sample_rate(uint32_t rate);

  /// @brief Get the sample rate in hz.
  /// @throws std::runtime_error if can't get the sample rate.
  uint32_t get_sample_rate() const;

  /// @brief Resets the USB endpoint buffer before starting to read samples.
  /// @note This function is mandatory before any reads.
  /// @note Call this function after device inizialization nd configuration.
  /// @throws nothing but print on `stderr` if someting goes wrong.
  void reset_buffer() noexcept;

  /// @brief Synchronous read until the data is available or and error occur.
  /// @param[in] buffer where to store the samples to read (multiple of 512).
  /// @param[out] read_count number of bytes readed, also if it has throw.
  /// @throws std::invalid_argument if buffer byte size is a not multiple of 512.
  /// @throws std::system_error of a `std::libusb_catgory` otherwise.
  /// @warning You must call `reset_buffer()` before this.
  /// @note Buffer size should be a multiple of 16'384, better 16 * 32 * 512.
  void read_sync(std::span<cu8_t> buffer, int& read_count);

  /// @brief
  template <typename Context = std::nullptr_t>
  void read_async(read_async_cb_t<Context>* cb,
                  Context& ctx,
                  std::size_t buffer_len = 0,
                  int buffer_num = 0);

  /// @brief
  void read_async(read_async_no_ctx_cb_t* cb,
                  std::size_t buffer_len = 0,
                  int buffer_num = 0);

  void cancel_async() noexcept;

private:

  template <typename Context>
  struct async_trampoline_t {
    read_async_cb_t<Context>* cb;
    Context& ctx;
  };

  template <typename Context>
  static void async_bridge(unsigned char* buf, uint32_t len, void* ctx)
  {
    std::span<cu8_t> buffer{reinterpret_cast<cu8_t*>(buf), len/2};
    auto* trampoline_ptr = static_cast<async_trampoline_t<Context>*>(ctx);

    trampoline_ptr->cb(
      buffer,
      trampoline_ptr->ctx
    );
  };

  static void async_bridge_no_ctx(unsigned char* buf, uint32_t len, void* ctx)
  {
    std::span<cu8_t> buffer{reinterpret_cast<cu8_t*>(buf), len/2};
    auto* cb = reinterpret_cast<read_async_no_ctx_cb_t*>(ctx);

    cb(buffer);
  }

//* Attributi privati / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

  rtlsdr_dev_t* handler_;
};


//* Implementations / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /


template <typename Context>
void dev_t::read_async(read_async_cb_t<Context>* cb,
                       Context& ctx,
                       std::size_t buffer_len,
                       int buffer_num)
{
  async_trampoline_t trampoline{cb, ctx};

  auto const error =
    rtlsdr_read_async(
      handler_,
      &dev_t::async_bridge<Context>,
      &trampoline,
      buffer_num,
      buffer_len
    );
  if (error) {
    throw std::runtime_error{"error reading asynchronously"};
  }
}


};  // end namespace rtlsdrpp


#endif  // JOHNHOP_RTLSDR_DEVT_HPP