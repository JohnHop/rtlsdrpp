#if !defined(JOHNHOP_RTLSDR_HPP)
#define JOHNHOP_RTLSDR_HPP


#include <string_view>
#include <string>

#include "dev_t.hpp"


namespace rtlsdrpp {


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
[[nodiscard]] device_info_t get_device_usb_strings(int index);


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