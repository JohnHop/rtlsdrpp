#if !defined(JOHNHOP_LIBUSBCATEGORY_HPP)
#define JOHNHOP_LIBUSBCATEGORY_HPP


#include <system_error>


namespace rtlsdr {


class libusb_category : public std::error_category
{
public:

  char const* name() const noexcept override {
    return "libusb";
  }

  /// @note From ``libusb-1.0-1 source code.
  std::string message(int ev) const override 
  {
    static char const* libusb_errors[] = {
      "Success",  // 0
		  "Input/Output Error", // 1
		  "Invalid parameter",  // 2
		  "Access denied (insufficient permissions)", // 3
		  "No such device (it may have been disconnected)", // 4
		  "Entity not found", // 5
		  "Resource busy",  // 6
		  "Operation timed out",  // 7
		  "Overflow", // 8
		  "Pipe error", // 9
		  "System call interrupted (perhaps due to signal)",  // 10
		  "Insufficient memory",  // 11
		  "Operation not supported or unimplemented on this platform",  // 12
		  "Other error",  // 13 -> -99
    };

    auto ev_index = -ev;
    if (ev_index < 0 || ev_index >= std::size(libusb_errors)) {
      ev_index = std::size(libusb_errors) - 1;
    }

    return libusb_errors[ev_index];
  }
};


[[nodiscard]] inline
std::error_category const& libusb_category() noexcept 
{
  static class libusb_category const instance;
  return instance;
}


};  // end namespace rtlsdr


#endif  // JOHNHOP_LIBUSBCATEGORY_HPP