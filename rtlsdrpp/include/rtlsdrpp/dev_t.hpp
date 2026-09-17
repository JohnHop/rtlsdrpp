#if !defined(JOHNHOP_RTLSDR_DEVT_HPP)
#define JOHNHOP_RTLSDR_DEVT_HPP


#include <vector>

#include <rtl-sdr.h>

#include "rtlsdrpp.hpp"


namespace rtlsdrpp {


/**
 * Wrapper around a `rtlsdr_dev_t` with RAII support.
 */
class dev_t
{
public:

  enum class turner_gain_mode_e {
    automatic, manual
  };

//* Special member functions / / / / / / / / / / / / / / / / / / / / / / / / / /

  /// @brief Inizialize a rtl-sdr device identified by `index`.
  /// @throws std::runtime_error if `index ` is not valid.
  dev_t(int index);

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
  /// @note This function print on the `stderr` if the real rate is different.
  void set_sample_rate(uint32_t rate);

  /// @brief Get the sample rate in hz.
  /// @throws std::runtime_error if can't get the sample rate.
  uint32_t get_sample_rate() const;

private:

  rtlsdr_dev_t* handler_;
};


};  // end namespace rtlsdrpp


#endif  // JOHNHOP_RTLSDR_DEVT_HPP