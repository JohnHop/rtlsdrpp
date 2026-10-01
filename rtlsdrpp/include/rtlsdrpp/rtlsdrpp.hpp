#if !defined(JOHNHOP_RTLSDR_HPP)
#define JOHNHOP_RTLSDR_HPP


#include <string_view>
#include <string>
#include <complex>


namespace rtlsdrpp {


// Samples types
using fc32_t = std::complex<float>;
using cu8_t = std::complex<uint8_t>;


/// Callbacks used in `read_async()`.
template <typename Context>
using read_async_cb_t = void(std::span<cu8_t> buffer, Context& ctx);

using read_async_no_ctx_cb_t = void(std::span<cu8_t> buffer);


/**
 * Hold some informations obtained with the `get_device_usb_strings()`.
 */
struct dev_info_t
{
  std::string manufact;
  std::string product;
  std::string serial;
};

/**
 * Hold some information about frequency value used in RTL2832 and turner IC.
 */
struct xtal_freq_t
{
  uint32_t rtl_freq;   // frequency value used to clock the RTL2832 in Hz.
  uint32_t tuner_freq; // requency value used to clock the tuner IC in Hz
};


enum class tuner_type_e {
	unknow,
	E4000,
	FC0012,
	FC0013,
	FC2580,
	R820T,
	R828D
};


/**
 * Tuner gain mode used by the `dev_t::set_turner_gain_mode()` method.
 */
enum class turner_gain_mode_e {
  automatic, manual
};


/**
 * Returns the number of RTL dongles attacched.
 *
 * @details
 * Now you can index from 0 up to return number -1 devices.
 */
[[nodiscard]] int get_device_count() noexcept;


/**
 * Returns the device name associated with a given index.
 *
 * @param[in] index The device index (starting from 0).
 * @throws std::system_error `ENODEV` If `index` is not a valid device index.
 */
[[nodiscard]] std::string_view get_device_name(int index);


/**
 * Returns some device info associated with a given index.
 *
 * @param[in] index The device index (starting from 0).
 * @throws std::system_error `rtlsdrpp::libusb_category()`.
 */
[[nodiscard]] dev_info_t get_device_usb_strings(int index);


/**
 * Get the device index from its serial.
 *
 * @param[in] serial Serial string of the device.
 * @throws std::invalid_argument If `serial` is empty.
 * @throws std::runtime_error If no device found.
 */
[[nodiscard]] int get_index_by_serial(std::string const& serial);


};


#endif  // JOHNHOP_RTLSDR_HPP