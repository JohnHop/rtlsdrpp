#if !defined(JOHNHOP_RTLSDR_DEVT_HPP)
#define JOHNHOP_RTLSDR_DEVT_HPP


#include <rtl-sdr.h>

#include "rtlsdr.hpp"


namespace rtlsdr {


/**
 * Wrapper around a `rtlsdr_dev_t` with RAII support.
 */
class dev_t
{
public:

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
  /// @note If `freq` < 500 khz use the direct sampling mode.
  void set_center_frequency(uint32_t freq);

  /// @brief Get the current center frequency.
  /// @throws std::runtime_error if can't get the frequency.
  uint32_t get_center_frequency() const;

private:

  rtlsdr_dev_t* handler_;
};


};  // end namespace rtlsdr


#endif  // JOHNHOP_RTLSDR_DEVT_HPP