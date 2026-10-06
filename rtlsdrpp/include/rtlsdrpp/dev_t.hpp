#if !defined(JOHNHOP_RTLSDR_DEVT_HPP)
#define JOHNHOP_RTLSDR_DEVT_HPP


#include <cstdint>
#include <vector>
#include <span>
#include <cstddef>
#include <stdexcept>

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

  /**
   * Initialize the rtl-sdr internal device handler identified by `index`.
   *
   * param[in] device index.
   * @throws std::runtime_error if `index` is not valid.
   */
  dev_t(int index = 0);

  /**
   * De-initialize a rtl sdr device.
   */
  ~dev_t();

//* Member functions / / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

  /**
   * Set the crystal oscillator frequencies used for device and tuner.

   * @param[in] frequency value used to clock the RTL2832 in hz.
   * @param[in] frequency value used to clock the tuner IC in hz.
   * @note Call this function only if you fully understand the implications.
   * @details
   * Usually both ICs use the same clock. Changing the clock may make sense if
   * you are applying an external clock to the tuner or to compensate the
   * frequency (and sample rate) error caused by the original (cheap) crystal.
   */
  void set_xtal_freq(xtal_freq_t xtal_freq);

  /**
   * Get crystal oscillator frequencies used for device and tuner IC.
   *
   * @returns both frequencies used by RTL2832 and tuerner IC.
   */
  xtal_freq_t get_xtal_freq() const noexcept;

  /// @brief Get the USB device strings.
  dev_info_t get_usb_strings() noexcept;

  /**
   * Write the device EEPROM.
   *
   * @param[in] data buffer of data to be written.
   * @param[in] offset address where the data should be written.
   */
  void write_eeprom(std::span<uint8_t> buffer, uint8_t offset);

  /**
   * Read the device EEPROM.
   */
  void read_eeprom(std::span<uint8_t> buffer, uint8_t offset) const;

  /**
   * Set the center frequency.
   *
   * @throws std::runtime_error if can't set the frequency.
  */
  void set_center_frequency(uint32_t freq);

  /**
   * Get the current center frequency.
   *
   * @throws std::runtime_error if can't get the frequency.
   */
  [[nodiscard]] uint32_t get_center_frequency() const;

  /**
   * Get the frequency correction value for the device.
   *
   * @returns correction value in parts per million (ppm).
   */
  [[nodiscard]] int get_freq_correction() const noexcept;

  /**
   * Set the frequency correction value for the device.
   *
   * @param[in] correction value in parts per million (ppm).
   */
  void set_freq_correction(int ppm);

  /**
   * Get the tuner type.
   */
  tuner_type_e get_turner_type() const noexcept;

  /**
   * Get a list of available gains for this rtl-sdr device.
   *
   * @note Result with a single gain value equals to 0 means no available gain.
   * @returns Available turner gain values in tenths of a dB.
   */
  [[nodiscard]] std::vector<int> get_turner_gains() const;

  /**
   * Set the gain using a value in tenths of a dB.
   *
   * @throws std::runtime_error if can't set the gain.
   * @note Manual gain mode must be enabled.
   * @note Remember to use `get_turner_gains` to get all available gains.
   */
  void set_tuner_gain(int gain);

  /**
   * Set the intermediate frequency gain for the device.
   *
   * @param[in] stage intermediate frequency gain stage number (1 to 6 for E4000).
   * @param[in] gain in tenths of a dB, -30 means -3.0 dB.
   */
  void set_turner_if_gain(int stage, int gain);

  /**
   * Set the tuner bandwidth in hz.
   *
   * @throws std::runtime_error if can't set the bandwidth.
   */
  void set_turner_bandwidth(uint32_t bw);

  /**
   * Get the tuner gain in tenths of a dB.
   *
   * @throws std::runtime_error if can't read the turner gain.
   * @note If `set_tuner_gain` failed the also this function will fail.
   */
  [[nodiscard]] int get_tuner_gain() const;

  /**
  * Set the tuner gain mode (automatic/manual).
  *
  * @throws std::runtime_error if can't set the turner gain mode.
  * @note This function succeeds also if it is not supported.
  */
  void set_turner_gain_mode(turner_gain_mode_e gain_mode);

  /**
  * Set the sample rate in hz.
  *
  * @param[in] rate Values can be: (225khz, 300 khz) and (900khz, 3200khz].
  * @throws std::invalid_argument if `rate` is invalid.
  * @warning Expect sample loss for rates > 2'400'000.
  * @note This function print on the `stderr` if the real rate is different.
  */
  void set_sample_rate(uint32_t rate);

  /**
   * Get the sample rate in hz.
   *
   * @throws std::runtime_error if can't get the sample rate.
   */
  [[nodiscard]] uint32_t get_sample_rate() const;

  /**
   * Enable test mode that returns an 8 bit counter instead of the samples.
   */
  void set_testmode(bool on);

  /**
   * Enable or disable the internal digital AGC of the RTL2832.
   */
  void set_agc_mode(bool on);

  /**
   * Enable or disable the direct sampling mode.
   *
   * @details
   * When enabled, the IF mode of the RTL2832 is activated, and
   * rtlsdr_set_center_freq() will control the IF-frequency of the DDC, which
   * can be used to tune from 0 to 28.8 MHz (xtal frequency of the RTL2832).
   */
  void set_direct_sampling(bool on);

  /**
   * Get state of the direct sampling mode.
   */
  direct_sampling_e get_direct_sampling() const noexcept;

  /**
   * Get state of the offset tuning mode.
   *
   * @returns `true` if enabled; `false` otherwise.
   */
  [[nodiscard]] bool get_offset_turning() const noexcept;

  /**
   * Enable or disable offset tuning for zero-IF tuners.
   *
   * @details Allows to avoid problems caused by the DC offset of the ADCs and 1/f noise.
   */
  void set_offset_tuning(bool on);

  /**
   * Resets the USB endpoint buffer before starting to read samples.
   *
   * @note This function is mandatory before any reads.
   * @note Call this function after device initialization nd configuration.
   * @throws nothing but print on `stderr` if something goes wrong.
   */
  void reset_buffer() noexcept;

  /**
   * Synchronous read until the data is available or and error occur.
   *
   * @param[in] buffer default size 262'144.
   * @param[out] read_count number of bytes read whenether if it has throw.
   * @throws std::invalid_argument if buffer byte size is a not multiple of 512.
   * @throws std::system_error of a `std::libusb_category` otherwise.
   * @warning You must call `reset_buffer()` before this.
   *
   * @note
   * Buffer size must be a multiple of 512, should be a multiple of 16'384,
   * the best is 16 * 32 * 512.
   */
  void read_sync(std::span<cu8_t> buffer, int& read_count);

  /**
   * Read samples from the device asynchronously.
   *
   * @param[in] cb read async callback invoked when samples are ready.
   * @param[in] ctx user specific context to pass via the callback function.
   * @param[in] buffer_num optional buffer count.
   * @param[in] buffer_len optional buffer length.
   * @note: blocks until it is canceled by `rtlsdr_cancel_async()`.
   */
  template <typename Context>
  void read_async(read_async_cb_t<Context>* cb,
                  Context& ctx,
                  std::size_t buffer_len = 131072,
                  int buffer_num = 15);

  /**
   *
   */
  void read_async(read_async_no_ctx_cb_t* cb,
                  std::size_t buffer_len = 131072,
                  int buffer_num = 15);

  /**
   * Cancel all pending asynchronous operations on the device.
   */
  void cancel_async() noexcept;


private:

//* Trampoline pattern / / / / / / / / / / / / / / / / / / / / / / / / / / / / /

  template <typename Context>
  struct async_trampoline_t {
    read_async_cb_t<Context>* cb;
    Context& ctx;
  };

  template <typename Context>
  static void async_bridge(unsigned char* buf, uint32_t buf_size, void* ctx)
  {
    std::span<cu8_t> buffer{reinterpret_cast<cu8_t*>(buf), buf_size/2};
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

//* Private data members / / / / / / / / / / / / / / / / / / / / / / / / / / / /

  rtlsdr_dev_t* handler_;
};


//* Private implementations / / / / / / / / / / / / / / / / / / / / / / / / / /

template <typename Context>
void dev_t::read_async(read_async_cb_t<Context>* cb,
                       Context& ctx,
                       std::size_t const buffer_len,
                       int const buffer_num)
{
  async_trampoline_t trampoline{cb, ctx};

  auto const error =
    rtlsdr_read_async(
      handler_,
      &dev_t::async_bridge<Context>,
      &trampoline,
      buffer_num,
      buffer_len*2
    );
  if (error) {
    throw std::runtime_error{"error reading asynchronously"};
  }
}


};  // end namespace rtlsdrpp


#endif  // JOHNHOP_RTLSDR_DEVT_HPP