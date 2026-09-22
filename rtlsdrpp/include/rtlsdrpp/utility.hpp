#if !defined(RTLSDRPP_CONVERSION_HPP)
#define RTLSDRPP_CONVERSION_HPP


#include <array>


namespace rtlsdrpp {


template <typename ContainerIn, typename ContainerOut>
constexpr void cu8tofc32(ContainerIn const& range_in, 
                         ContainerOut& range_out) noexcept
{
  auto lut_init = []() 
  {
    constexpr auto offset = 127.5f;
    constexpr auto scale = 1 / 127.5f;

    std::array<float,256> lut;
    for (int i = 0; i < 256; ++i) {
      lut[i] = (static_cast<float>(i) - offset) * scale;
    }

    return lut;
  };

  constexpr auto lut = lut_init();  // lookup table

  // conversion
  for (int i = 0; i < range_out.size(); ++i)
  {
    range_out[i] = {
      lut[range_in[2*i]],
      lut[range_in[2*i + 1]]
    };
  }
};


};


#endif  // RTLSDRPP_CONVERSION_HPP