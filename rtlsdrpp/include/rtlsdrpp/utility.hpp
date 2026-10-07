#if !defined(RTLSDRPP_CONVERSION_HPP)
#define RTLSDRPP_CONVERSION_HPP


#include <algorithm>
#include <ranges>
#include <cassert>
#include <chrono>

#include "rtlsdrpp.hpp"


namespace rtlsdrpp {


/**
 * Easily convert cu8 format to fc32.
 *
 * @note: `RangeIn` and `RangeOut` must be same size.
 */
template <
  std::ranges::input_range RangeIn,
  std::ranges::output_range<fc32_t> RangeOut
>
constexpr void cu8tofc32(RangeIn const& range_in,
                         RangeOut& range_out) noexcept
{
  assert(range_in.size() == range_out.size());

  auto lut_init = []()
  {
    constexpr auto offset = 127.5f;
    constexpr auto scale = 1 / 128.0f;

    std::array<float,256> lut;
    for (int i = 0; i < 256; ++i) {
      lut[i] = (static_cast<float>(i) - offset) * scale;
    }

    return lut;
  };

  constexpr auto lut = lut_init();  // lookup table at compile time

  // conversion
  std::ranges::transform(
    range_in,
    range_out.begin(),
    [&lut](cu8_t const cu8) -> fc32_t {
      return {lut[cu8.real()], lut[cu8.imag()]};
    }
  );
};


/**
 * Easily obtain the number of I/Q samples (not buffer size in byte!) required
 * to stream at specified sample rate for some milliseconds.
 */
[[nodiscard]] constexpr
std::size_t nsamples(std::chrono::milliseconds ms,
                     double const sample_rate) noexcept
{
  return ms.count() * (sample_rate / 1'000);
}


};


#endif  // RTLSDRPP_CONVERSION_HPP